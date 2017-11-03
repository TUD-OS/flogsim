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
if (file.access(trace.filename) == -1) {
    stop(sprintf("Specified model file (%s) does not exist", trace.filename))
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
trace.df[, (event.cols) := lapply(.SD, function(x) strsplit(x, " ")) ,.SDcols = event.cols]
trace.df <- melt(trace.df, id.vars=cpu.col)
trace.df <- unnest(trace.df, value)

trace.df[, variable := as.character(variable)]

trace.df[, c("variable", "field") := colsplit(trace.df$variable, "_", c("event", "field"))]

cols <- c("value", "field")
trace.df[, (cols) := lapply(.SD, function(x) strsplit(x, "\\|")) ,.SDcols = cols]
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
msg.start <- trace.df[, .SD[variable %in% c("CpuEvent"),
                            .(From = CPU, Start = End, Tag = Tag)],
                      by = Sequence]
msg.start <- msg.start[, .SD[which.min(Start)], by = Sequence]
msg.end = trace.df[, .SD[variable %in% c("RecvGap", "Failure"), .(To = CPU, End = Time, variable = variable)], by = Sequence]

messages <- msg.start[msg.end, on = "Sequence"]

full.stop <- trace.df[variable=="Finish", max(Time)]

variables <- c("CpuEvent" = 2, "SendGap" = 3, "RecvGap" = 4, "Failure" = 5, "Finish" = 1,
               "0" = 1, "2" = 2, "4" = 3)

## Model specific part ended

major.breaks <- seq(0, model.df$P + 4, 5)
minor.breaks <- seq(min(range(major.breaks)), max(range(major.breaks)))

pdf(opt$options$output, width=20, height=model.df$P / 4)
p <- ggplot(trace.df[!(variable %in% c("CpuEvent", "Finish", "Failure"))],
            aes(ymin = as.double(Time), ymax = as.double(End), x = CPU, col = variable)) +
    geom_linerange(alpha = 0.3, size = 2) +
    geom_linerange(data = trace.df[variable %in% c("CpuEvent", "Finish", "Failure")],
                   size = 1, aes(x = CPU + 0.2)) +
    geom_hline(yintercept = full.stop, size = 1) +
    scale_colour_manual(name = "Event type", values = variables) +
    coord_flip() +
    scale_x_continuous(breaks = minor.breaks, labels = minor.breaks, limits = c(0, max(model.df$P)))

if (opt$options$cpu_only == TRUE) {
    print(p)
} else if (is.null(interesting.nodes)) {
    print(p + geom_segment(data = messages,
                           aes(x = From + 0.05, xend = To + 0.05, y = Start, yend = End, col = as.factor(Tag)),
                           arrow = arrow(length = unit(0.01, "npc")), inherit.aes = FALSE))
} else {
    print(p + geom_segment(data = messages[From %in% interesting.nodes | To %in% interesting.nodes],
                           aes(x = From + 0.05, xend = To + 0.05, y = Start, yend = End, col = as.factor(Tag)),
                           arrow = arrow(length = unit(0.01, "npc")), inherit.aes = FALSE) +
          ggtitle("Highlighted nodes:", subtitle = paste(interesting.nodes, collapse = ", ")))
}

res <- dev.off()
