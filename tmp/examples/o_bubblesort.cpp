#include "o_reg.inc"

#define SIZE 32
#define LEN 6

using namespace std;

void bubbleSort(SecureInt[], int);
void printArray(SecureInt[], int);
void swap(SecureInt*, SecureInt*);
 
// Driver program to test above functions
int mainz()
{
	SecureInt a[LEN];
	a[0] = SecureInt::encrypt(7);
	a[1] = SecureInt::encrypt(12);
	a[2] = SecureInt::encrypt(2);
	a[3] = SecureInt::encrypt(0);
	a[4] = SecureInt::encrypt(1);
	a[5] = SecureInt::encrypt(15);

	//printArray(a, LEN);

	asm("l.debug");
	
	bubbleSort(a, LEN);

	cout << "Sorted array is:  ";
	printArray(a, LEN);

	asm("l.debug");
	return 0;
}

// A function to implement bubble sort
void bubbleSort(SecureInt arr[], int n)
{
	int i, j;
	SecureInt cond1, cond2, smaller, greater;

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
void printArray(SecureInt arr[], int size)
{
	for (int i=0; i < size; i++) cout << arr[i].str() << " ";
	cout << "\n";
}


void swap(SecureInt *xp, SecureInt *yp)
{
	SecureInt temp = *xp;
	*xp = *yp;
	*yp = temp;
}

