
#ifndef _MCP4921_H
#define _MCP4921_H

class MCP4921 {
public:
  MCP4921();
  void write(unsigned int value);
private:
  void cycle_clock();
  int fword;
};

#endif //_MCP4921_H
