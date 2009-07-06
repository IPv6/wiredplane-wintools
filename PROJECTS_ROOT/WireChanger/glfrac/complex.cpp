/* A library for complex functions */
#include "complex.h"
#include <stdio.h>
#include <math.h>

CMPLX create_complex (double real, double imag)
/* Create a complex number */
{
    CMPLX new_no;  /* New complex number created */
    new_no.real= real;
    new_no.imag= imag;
    return new;
}

CMPLX add (CMPLX a, CMPLX b)
/* Add two complex numbers a+b*/
{
    CMPLX answer;  /* Addition of a and b */
    answer.real= a.real+b.real;
    answer.imag= a.imag+b.imag;
    return answer;
}

CMPLX subtract (CMPLX a, CMPLX b)
/* Subtract two complex numbers a-b */
{
    CMPLX answer;
    answer.real= a.real - b.real;
    answer.imag= a.imag - b.imag;
    return answer;

}

CMPLX multiply (CMPLX a, CMPLX b)
/* Multiply two complex numbers a*b */
{
    CMPLX answer;
    answer.real= a.real*b.real - a.imag*b.imag;
    answer.imag= a.imag*b.real + a.real*b.imag;
    return answer;
}

void print_complex (CMPLX a)
/* Print out a complex number */
{
    /* the if statement ensures we don't get things like 5+-3i printed */
    if (a.imag > 0)
        printf ("(%g+%gi)", a.real, a.imag);
    else
        printf ("(%g%gi)",a.real, a.imag);
}


double magnitude (CMPLX a)
/* Print the magnitude of a complex number a*/
{
    return (sqrt(a.real*a.real+a.imag*a.imag));
}
/*
{
    CMPLX x,y,z;
    x= create_complex(5.3, 4.1);
    y= create_complex(6.3, -2.1);
    z= add (x,y);
    print_complex (x);
    printf (" + ");
    print_complex (y);
    printf (" = ");
    print_complex (z);
    printf ("\n");
    z= subtract (x,y);
    print_complex (x);
    printf (" - ");
    print_complex (y);
    printf (" = ");
    print_complex (z);
    printf ("\n");
    return 0;
}

*/