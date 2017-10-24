suppressMessages(library(ggplot2))
suppressMessages(library(purrr))
suppressMessages(library(data.table))
suppressMessages(library(tidyr))
suppressMessages(library(reshape2))

trace.filename <- 'log.trace.csv'
model.filename <- 'log.model.csv'

trace.df <- fread(trace.filename, colClasses = "character")
model.df <- fread(model.filename)

event.cols <- names(trace.df)[-1]
cpu.col <- names(trace.df)[1]

## Tidy the data table
trace.df[, (event.cols) := map(.SD, ~strsplit(., " ")) ,.SDcols = event.cols]
trace.df <- melt(trace.df, id.vars=cpu.col)
trace.df <- unnest(trace.df, value)

trace.df[, variable := as.character(variable)]

trace.df[, c("variable", "field") := colsplit(trace.df$variable, "_", c("event", "field"))]

cols <- c("value", "field")
trace.df[, (cols) := map(.SD, ~strsplit(., "\\|")) ,.SDcols = cols]
trace.df[, id := 1:nrow(.SD)]
trace.df <- unnest(trace.df, value, field)

trace.df[, value := as.double(value)]
trace.df[, CPU := as.integer(CPU)]

trace.df <- spread(trace.df, field, value)

# Enforce order

trace.df$End = 0
trace.df[variable=="CpuEvent", End := Time + model.df$o]
trace.df[variable=="SendGap", End := Time + model.df$g]
trace.df[variable=="RecvGap", End := Time + model.df$g]
trace.df[variable=="Finish", End := Time + 0.1]
trace.df[variable=="Failure", End := Time + 0.1]

## End of CpuEvent is start of latency
msg.start <- trace.df[, .SD[variable %in% c("CpuEvent"), .(From = CPU, Start = End)], by = Sequence]
msg.start <- msg.start[, .SD[which.min(Start)], by = Sequence]
msg.end = trace.df[, .SD[variable %in% c("RecvGap", "Failure"), .(To = CPU, End = Time, variable = variable)], by = Sequence]

messages <- msg.start[msg.end, on = "Sequence"]

full.stop <- trace.df[variable=="Finish", max(Time)]

variables <- c("CpuEvent" = 2, "SendGap" = 3, "RecvGap" = 4, "Failure" = 5, "Finish" = 1)

## Model specific part ended

major.breaks <- seq(0, model.df$P + 4, 5)
minor.breaks <- seq(min(range(major.breaks)), max(range(major.breaks)))

pdf("plot.pdf", width=20, height=model.df$P / 4)
p <- ggplot(trace.df[!(variable %in% c("CpuEvent", "Finish", "Failure"))],
            aes(ymin = as.double(Time), ymax = as.double(End), x = CPU, col = variable)) +
    geom_linerange(alpha = 0.3, size = 2) +
    geom_linerange(data = trace.df[variable %in% c("CpuEvent", "Finish", "Failure")],
                   size = 1, aes(x = CPU + 0.2)) +
    geom_hline(yintercept = full.stop, size = 1) +
    scale_colour_manual(name = "Event type", values = variables) +
    coord_flip() +
    scale_x_continuous(breaks = minor.breaks, labels = minor.breaks, limits = c(0, max(model.df$P)))

print(p + geom_segment(data = messages,
                 aes(x = From + 0.05, xend = To + 0.05, y = Start, yend = End, col = variable),
                 arrow = arrow(length = unit(0.01, "npc")), inherit.aes = FALSE))
print(p)
dev.off()
