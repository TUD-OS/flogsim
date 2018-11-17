#pragma once

class Timeline;
class FaultInjector;

class ResultsPrinter
{
protected:
  virtual void print_header();
  virtual void print_metrics();
  // Find a metric value by key
  virtual std::string metric_value(const std::string &key);
public:
  virtual void intro() {}
  virtual void results(Timeline &timeline, FaultInjector &faults) = 0;

  static std::unique_ptr<ResultsPrinter> create();
};

class TablePrinter : public ResultsPrinter
{
  void print_metrics() override;
public:
  void results(Timeline &timeline, FaultInjector &faults) override;
};

class CsvPrinter : public ResultsPrinter
{
public:
  void intro() override;
  void results(Timeline &timeline, FaultInjector &faults) override;
};

class CsvIdPrinter : public ResultsPrinter
{
public:
  void intro() override;
  void results(Timeline &timeline, FaultInjector &faults) override;
};

// Printer that allows to specify which exactly columns are about to
// be printed. If a column is not available for a particular
// experiment type, an empty string will be printed instead.
//
// The format is useful to be able to combine multiple experiments in
// one table.
class CsvColumnsPrinter : public ResultsPrinter
{
  std::vector<std::string> header;
public:
  CsvColumnsPrinter();

  void intro() override;
  void results(Timeline &timeline, FaultInjector &faults) override;
};
