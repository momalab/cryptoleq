#include <iostream>
#include <stdlib.h>

#define LEN 6

using namespace std;

void bubbleSort(int[], int);
void printArray(int[], int);
void swap(int*, int*);
 
// Driver program to test above functions
int main()
{
	int a[LEN];
	a[0] = rand();
	a[1] = rand();
	a[2] = rand();
	a[3] = rand();
	a[4] = rand();
	a[5] = rand();

	asm("l.debug");
	
	bubbleSort(a, LEN);

	cout << "Sorted array is:  ";
	printArray(a, LEN);

	asm("l.debug");
	return 0;
}

// A function to implement bubble sort
void bubbleSort(int arr[], int n)
{
	int i, j;
	int cond1, cond2, smaller, greater;

	for (i = 0; i < n-1; i++)
	{
		// Last i elements are already in place   
		for (j = 0; j < n-i-1; j++)
		{
			cond1 = arr[j] <= arr[j+1];
			cond2 = arr[j+1] < arr[j]; // remove if ternary
			smaller = cond1 * arr[j] + cond2 * arr[j+1];
			//smaller = cond1.ternary(arr[j], arr[j+1]);
			greater = arr[j+1] + arr[j] - smaller;
			arr[j] = smaller;
			arr[j+1] = greater;
		}
	}
}


/* Function to print an array */
void printArray(int arr[], int size)
{
	for (int i=0; i < size; i++) cout << arr[i] << " ";
	cout << "\n";
}


void swap(int *xp, int *yp)
{
	int temp = *xp;
	*xp = *yp;
	*yp = temp;
}

