
#ifndef _MCP3202_H
#define _MCP3202_H

class MCP3202 {
public:
  MCP3202();
  unsigned int read(unsigned int channel);
private:
  void cycle_clock();
};

#endif //_MCP3202_H
