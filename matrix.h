#ifndef MATRIX_H
#define MATRIX_H

typedef struct
{
	float val[3][3];
}
mat3x3;

typedef struct
{
	float val[1][3];
}
mat1x3;

typedef union
{
	mat1x3 mat;
	struct
	{
		float x;
		float y;
		float z;
	};
}
point3d;

mat1x3 mat_mult_1x3_3x3(const mat1x3 * a, const mat3x3 * b);

mat3x3 mat_mult_3x3_3x3(const mat3x3 * a, const mat3x3 * b);

mat1x3 mat_cross_mult_1x3(const mat1x3 * a, const mat1x3 * b);

float mat_dot_mult_1x3(const mat1x3 * a, const mat1x3 * b);

mat1x3 mat_add_1x3(const mat1x3 * a, const mat1x3 * b);

mat1x3 mat_sub_1x3(const mat1x3 * a, const mat1x3 * b);

#endif
