#pragma once

class Timeline;
class FaultInjector;

class ResultsPrinter
{
public:
  virtual void intro() {}
  virtual void results(Timeline &timeline, FaultInjector &faults) = 0;

  static std::unique_ptr<ResultsPrinter> create();
};

class TablePrinter : public ResultsPrinter
{
public:
  void results(Timeline &timeline, FaultInjector &faults) override;
};
