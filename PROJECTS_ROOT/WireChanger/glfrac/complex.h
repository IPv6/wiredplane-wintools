typedef struct complex {
    double real; /* Real part of number */
    double imag; /* Imaginary part of number */
} CMPLX;

CMPLX create_complex (double, double);
/* Create a complex number */
CMPLX add (CMPLX, CMPLX);
/* Add two complex numbers */
CMPLX subtract (CMPLX, CMPLX);
/* Subtract two complex numbers */
CMPLX multiply (CMPLX, CMPLX);
/* Multiply two complex numbers */
void print_complex (CMPLX);
/* Print out a complex number */
double magnitude (CMPLX);
/* Print the magnitude of a complex number */
