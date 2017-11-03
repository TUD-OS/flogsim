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

pdf(output.pdf, 16, 9)
for (cur.L in unique(log.df$L)) {
    ymax = max(log.df$TotalRuntime)
    p <- ggplot(log.df[L == cur.L], aes(x = P, y = TotalRuntime, color = Collective)) +
        geom_point() + geom_line() +
        scale_x_log10(breaks = 2^c(1:11) - 1, labels = 2^c(1:11) - 1) +
        ylim(0, ymax) +
        ggtitle("Scalability in failure free case",
                subtitle=sprintf("L = %d, o = 1, g = 1", cur.L)) +
        scale_colour_manual(labels = c("Binary Broadcast",
                                       "Eager Checked Corrected Tree Broadcast",
                                       "Fixed Corrected Tree Broadcast",
                                       "Phased Checked Corrected Tree Broadcast"),
                            values = c(1:4)) +
        theme(legend.position = c(.85, .35))
    print(p)
}

sink <- dev.off()
