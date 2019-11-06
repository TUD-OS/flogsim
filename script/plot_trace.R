#!/usr/bin/env Rscript

suppressMessages(library(ggplot2))
suppressMessages(library(data.table))
suppressMessages(library(tidyr))
suppressMessages(library(reshape2))
suppressMessages(library(optparse))

## Reading the options

option.list <- list(
    make_option(c("-c", "--cpu-only"), action = "store_true", default = FALSE,
                help  = "Show only events on the nodes, omit messages."),
    make_option(c("--messages"), default = NULL, metavar = "LIST",
                help = "Show messages only on selected nodes (e. g. '0,7')."),
    make_option(c("-m", "--model"), metavar = "FILE",
                help = "Path to the file with model description."),
    make_option(c("-o", "--output"), default = "plot.pdf", metavar = "FILE",
                help = "Output PDF file. [default: %default]"))

opt.parser <- OptionParser(option_list = option.list,
                           usage = "%prog -m MODEL [options] TRACE")
opt <- parse_args2(opt.parser)

trace.filename <- opt$args[1]
if (file.access(trace.filename) == -1) {
    stop(sprintf("Specified trace file (%s) does not exist", trace.filename))
}

model.filename <- opt$options$model
if (file.access(model.filename) == -1) {
    stop(sprintf("Specified model file (%s) does not exist", model.filename))
}

if (!is.null(opt$options$messages)) {
    interesting.nodes <- as.integer(unlist(strsplit(opt$options$messages, ",")))
} else {
    interesting.nodes <- NULL
}

## Reading the model and the trace

model.df <- fread(model.filename)
trace.df <- fread(trace.filename, colClasses = "character")


## Start visualisation

event.cols <- names(trace.df)[-1]
cpu.col <- names(trace.df)[1]

## Tidy the data table
trace.df[, (event.cols) := lapply(.SD, function(x) strsplit(gsub("\\s+", " ", x), " ")) ,.SDcols = event.cols]
trace.df <- melt(trace.df, id.vars=cpu.col)
trace.df <- unnest(trace.df, value)
trace.df <- as.data.table(trace.df)

trace.df[, variable := as.character(variable)]

trace.df[, c("variable", "field") := colsplit(trace.df$variable, "_", c("event", "field"))]

cols <- c("value", "field")
trace.df[, (cols) := lapply(.SD, function(x) strsplit(x, "\\|")) ,.SDcols = cols]
trace.df[, id := 1:nrow(.SD)]
trace.df <- unnest(trace.df, cols)
trace.df <- as.data.table(trace.df)

trace.df[, CPU := as.integer(CPU)]

trace.df <- spread(trace.df, field, value)

trace.df[, Time := as.double(Time)]
trace.df[, Sequence := as.integer(Sequence)]

# Enforce order

trace.df$End = 0.
trace.df[variable=="CpuEvent", End := Time + model.df$o]
trace.df[variable=="SendGap", End := Time + model.df$g]
trace.df[variable=="RecvGap", End := Time + model.df$g]
trace.df[variable=="Finish", End := Time + 0.1]
trace.df[variable=="Failure", End := Time + 0.1]

# Convert tags to something more readable
trace.df$Tag <- factor(trace.df$Tag)
levels(trace.df$Tag) <- c(
    "right_ring"="Right",
    "left_ring"="Left",
    "gossip"="Gossip",
    "tree"="Tree",
    "gather" = "Gather"
    )

## End of CpuEvent is start of latency
msg.start <- trace.df[, .SD[variable %in% c("CpuEvent"),
                            .(From = CPU, Start = End, Tag = Tag)],
                      by = Sequence]
msg.start <- msg.start[, .SD[which.min(Start)], by = Sequence]
msg.end = trace.df[, .SD[variable %in% c("RecvGap", "Failure"), .(To = CPU, End = Time, variable = variable)], by = Sequence]

messages <- msg.start[msg.end, on = "Sequence"]
messages <- na.omit(messages)

full.stop <- trace.df[variable=="Finish", max(Time)]

message.variables <- c("Gossip" = "red", "Tree" = "black", "Left" = "#cfcf33", "Right" = "#a65628")
event.variables <- c("CpuEvent" = '#fc8d62', "SendGap" = '#66c2a5', "RecvGap" = '#8da0cb', "Failure" = '#984ea3', "Finish" = 'black')
message.breaks <- c("Tree", "Gossip", "Left", "Right")
message.labels <- c("Tree" = "Tree", "Gossip" = "Gossip", "Left" = "Ring left", "Right" = "Ring right")

breaks <- c("CpuEvent", "RecvGap", "SendGap",  "Failure", "Finish")
labels <- c("CPU (o)", "Receive gap (g)", "Send gap (g)",  "Failure", "Finish")

## Model specific part ended

major.breaks <- seq(0, model.df$P + 4, 5)
minor.breaks <- seq(min(range(major.breaks)), max(range(major.breaks)))

pdf(opt$options$output, width=20, height=max(4, model.df$P / 6))
p <- ggplot(trace.df[!(variable %in% c("CpuEvent", "Finish", "Failure"))],
            aes(xmin = as.double(Time), xmax = as.double(End), ymin = CPU, ymax = CPU + 0.5, fill = variable)) +
    geom_rect(alpha = 0.8, size = 0.1, col = 'black') +
    geom_rect(data = trace.df[variable %in% c("CpuEvent")],
                 size = 0.1, aes(ymin = CPU + 0.5, ymax = CPU + 0.7), col = 'black') +
    geom_rect(data = trace.df[variable %in% c("Finish", "Failure")],
                 size = 0.1, aes(ymin = CPU + 0.0, ymax = CPU + 0.7), col = 'black') +
    geom_vline(xintercept = full.stop, size = 1) +
    scale_fill_manual(name = "Event type", breaks=breaks, labels=labels, values = event.variables) +
    scale_y_continuous(breaks = minor.breaks, labels = minor.breaks, limits = c(0, max(model.df$P)))

arrow.type <- arrow(type = 'closed', angle = 15, length = unit(0.01, "npc"))
if (opt$options$cpu_only != TRUE) {
    if (is.null(interesting.nodes)) {
        p <- p + geom_segment(data = messages,
                              aes(y = From + 0.25, yend = To + 0.25, x = Start, xend = End, col = Tag),
                              arrow = arrow.type, inherit.aes = FALSE)
    } else {
        p <- p + geom_segment(data = messages[From %in% interesting.nodes | To %in% interesting.nodes],
                              aes(y = From + 0.25, yend = To + 0.25, x = Start, xend = End, col = Tag),
                              arrow = arrow.type, inherit.aes = FALSE) +
            ggtitle("Highlighted nodes:", subtitle = paste(interesting.nodes, collapse = ", "))
    }
}

print(p + xlab("Time") + ylab("CPU") +
      scale_colour_manual(name = "Message type", breaks=message.breaks, labels=message.labels, values = message.variables) +
      theme_linedraw() + theme(panel.grid.minor = element_blank(),
                               panel.grid.major = element_line(colour='gray')))


res <- dev.off()
