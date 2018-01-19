#pragma once

class Timeline;
class FaultInjector;

class ResultsPrinter
{
protected:
  virtual void print_header();
  virtual void print_metrics();
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
