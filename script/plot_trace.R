library(ggplot2)
library(purrr)
library(data.table)
library(bit64)
library(tidyr)
library(reshape2)

trace.filename <- 'log.trace.csv'
model.filename <- 'log.model.csv'

trace.df <- fread(trace.filename, colClasses = "character")

event.cols <- names(trace.df)[-1]
cpu.col <- names(trace.df)[1]

trace.df[, (event.cols) := map(.SD, ~strsplit(., " ")) ,.SDcols = event.cols]
trace.df[, (event.cols) := map(.SD, ~map(., function(x) as.integer64(x))) ,.SDcols = event.cols]
trace.df[, (cpu.col) := as.integer(get(cpu.col))]

trace.df <- melt(trace.df, id.vars=cpu.col)
trace.df <- unnest(trace.df, value)
names(trace.df) <- c(cpu.col, "event", "start")
trace.df[, end := start]

## Now I write very model specific code. And I expect LogP model
model.df <- fread(model.filename)

trace.df[event=="CpuEvent", end := start + model.df$o]
trace.df[event=="SendGaps", end := start + model.df$g]
trace.df[event=="RecvGaps", end := start + model.df$g]

events <- c("CpuEvent" = 2, "SendGaps" = 3, "RecvGaps" = 4, "Finish" = 5)

## Model specific part ended

ggplot(trace.df[event!="CpuEvent"],
       aes(ymin = as.double(start), ymax = as.double(end), x = CPU, col = event)) +
    geom_linerange(alpha = 0.5, size = 4) +
    geom_linerange(data = trace.df[event=="CpuEvent"], size = 4) +
    scale_colour_manual(name = "Event type", values = events) +
    coord_flip()
