#include "pch.h" 
#include <utility>
#include <limits.h>
#include "CppDll.h"



static unsigned long long previous_;  // Previous value, if any
static unsigned long long current_;   // Current sequence value
static unsigned index_;               // Current seq. position

/* Maska
	1	0	-1
	1	1	-1
	1	0	-1
*/


void filterCPP(BYTE* data, BYTE* output, int width, int padding, int start, int count) {
	
	int s = 0;

	BYTE* bottomRow = data + (start-1) * (padding + width * 3);
	BYTE* dataRow = data + start * (padding + width * 3);
	BYTE* upperRow = data + (start+1) * (padding + width * 3);

	BYTE* currentRow = output + start * (padding + width * 3);

	for (int i = 0; i < count; i++) {
		for (int j = 3; j < (width - 2) * 3; j += 3) {
			for (int k = 0; k < 3; k++) {

				s = (int)(bottomRow[(j - 3) + k] - bottomRow[(j + 3) + k]
					+ dataRow[(j - 3) + k] + dataRow[j + k] - dataRow[(j + 3) + k]
					+ upperRow[(j - 3) + k] - upperRow[(j + 3) + k]);
				if (s < 0)
					s = 0;
				if (s > 255)
					s = 255;

				currentRow[j + k] = s;
			}
		}
		bottomRow = bottomRow + (padding + width * 3) ;
		dataRow = dataRow + (padding + width * 3) ;
		upperRow = upperRow + (padding + width * 3) ;
		currentRow = currentRow + (padding + width * 3) ;
	}
}

