#!/usr/bin/env Rscript

suppressMessages(library(ggplot2))
suppressMessages(library(data.table))
suppressMessages(library(tidyr))
suppressMessages(library(reshape2))
suppressMessages(library(optparse))
suppressMessages(library(scales))

## Reading the options

option.list <- list(
    make_option(c("-o", "--output"), default = "failure_free.pdf", metavar = "FILE",
                help = "Output PDF file. [default: %default]"))

opt.parser <- OptionParser(option_list = option.list,
                           usage = "%prog [options] LOG")
opt <- parse_args2(opt.parser)

log.filename <- opt$args[1]
if (file.access(log.filename) == -1) {
    stop(sprintf("Specified log file (%s) does not exist", log.filename))
}

output.pdf <- opt$options$output

## Reading the model and the log

log.df <- fread(log.filename)

node.list <- 2^c(1:13)

pdf(output.pdf, 8, 5)
for (cur.L in unique(log.df$L)) {
    ymax = max(log.df$TotalRuntime)
    p <- ggplot(log.df[L == cur.L], aes(x = P, y = TotalRuntime, color = Collective)) +
        geom_point() + geom_line() +
        scale_x_log10(breaks = node.list - 1, labels = node.list - 1) +
        ylim(0, ymax) + ylab("Total Runtime") +
        ggtitle("Scalability in failure free case",
                subtitle=sprintf("L = %d, o = 1, g = 1", cur.L)) +
        scale_colour_manual(labels = c("Binary Broadcast",
                                       "Eager Checked Corrected Tree Broadcast",
                                       "Fixed Corrected Tree Broadcast",
                                       "Phased Checked Corrected Tree Broadcast"),
                            values = c(1:4)) +
        theme(legend.position = c(.8, .3))
    print(p)
}

for (cur.L in unique(log.df$L)) {
    ymax = max(log.df$MsgTask)
    p <- ggplot(log.df[L == cur.L], aes(x = P, y = MsgTask, color = Collective)) +
        geom_point() + geom_line() +
        scale_x_log10(breaks = node.list - 1, labels = node.list - 1) +
        ylim(0, ymax) + ylab("Message count") +
        ggtitle("Scalability in failure free case (log)",
                subtitle=sprintf("L = %d, o = 1, g = 1", cur.L)) +
        scale_colour_manual(labels = c("Binary Broadcast",
                                       "Eager Checked Corrected Tree Broadcast",
                                       "Fixed Corrected Tree Broadcast",
                                       "Phased Checked Corrected Tree Broadcast"),
                            values = c(1:4)) +
        theme(legend.position = c(.25, .35))
    print(p)
}

for (cur.L in unique(log.df$L)) {
    ymax = max(log.df$MsgTask)
    p <- ggplot(log.df[L == cur.L], aes(x = P, y = MsgTask, color = Collective)) +
        geom_point() + geom_line() +
        scale_x_log10(breaks = node.list - 1, labels = node.list - 1) +
        scale_y_log10() + ylab("Total number of messages") +
        ggtitle("Scalability in failure free case (log-log)",
                subtitle=sprintf("L = %d, o = 1, g = 1", cur.L)) +
        scale_colour_manual(labels = c("Binary Broadcast",
                                       "Eager Checked Corrected Tree Broadcast",
                                       "Fixed Corrected Tree Broadcast",
                                       "Phased Checked Corrected Tree Broadcast"),
                            values = c(1:4)) +
        theme(legend.position = c(.75, .25))
    print(p)
}

sink <- dev.off()
