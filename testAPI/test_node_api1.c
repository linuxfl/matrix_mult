#include <stdio.h>
#include "MPI_node.h"

int main()
{
	MPIN_init("./hosts",10);
	return 0;
}
