
#ifndef _MCP3208_H
#define _MCP3208_H

class MCP3208 {
public:
  MCP3208();
  unsigned int read(unsigned int channel);
private:
  void cycle_clock();
};

#endif //_MCP3208_H
