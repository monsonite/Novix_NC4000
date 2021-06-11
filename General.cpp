#include "general.h"


memory::memory(unit s) {
	max_adr=s;
	a = new unit[s+1];
	for (int i=0;i<=s;i++) a[i]=0;

        bp = false;
}

memory::~memory(void) {}

unit memory::read(unit adr) {
	if (adr<=max_adr) return a[adr];
	return 0;
}

void memory::write(unit adr, unit data)
{
  if (adr == 0x22) {
    bp = true;
  }

  if (adr<=max_adr) a[adr]=data;
}

int memory::get_size(void) {
	return max_adr+1;
}

bool memory::do_break (void)
{
  bool res = bp;

  return (false);

  bp = false;
  return (res);
}