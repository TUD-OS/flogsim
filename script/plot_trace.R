library(ggplot2)
library(purrr)
library(data.table)
library(bit64)
library(tidyr)
library(reshape2)

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

trace.df[order(Time)]

trace.df[, End := Time]
trace.df[, Start := Time]
trace.df[variable=="CpuEvent", End := Time + model.df$o]
trace.df[variable=="SendGap", End := Time + model.df$g]
trace.df[variable=="RecvGap", End := Time + model.df$g]
trace.df[variable=="RecvGap", Start := Start - model.df$L]
trace.df[variable=="Finish", End := Time + 0.1]
trace.df[variable=="Failure", End := Time + 0.1]
trace.df[variable=="Failure", Start := Start - model.df$L]

full.stop <- trace.df[variable=="Finish", max(Time)]

variables <- c("CpuEvent" = 2, "SendGap" = 3, "RecvGap" = 4, "Failure" = 5, "Finish" = 1)

## Model specific part ended

#pdf("sth.pdf", width=20, height=64)
ggplot(trace.df[!(variable %in% c("CpuEvent", "Finish", "Failure"))],
       aes(ymin = as.double(Time), ymax = as.double(End), x = CPU, col = variable)) +
    geom_linerange(alpha = 0.3, size = 4) +
    geom_linerange(data = trace.df[variable %in% c("CpuEvent", "Finish", "Failure")],
                   size = 4, aes(x = CPU + 0.1)) +
    geom_hline(yintercept = full.stop, size = 1) +
    geom_segment(data = trace.df[!is.na(Sender)], aes(x = Sender + 0.05, xend = CPU + 0.05, y = Start, yend = Time, col = variable),
                 arrow = arrow(length = unit(0.01, "npc"))) +
    scale_colour_manual(name = "Event type", values = variables) +
    coord_flip()
#dev.off()
