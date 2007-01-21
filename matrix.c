#include <math.h>
#include "matrix.h"

mat1x3 mat_mult_1x3_3x3(const mat1x3 * a, const mat3x3 * b)
{
	mat1x3 out = {{
		a->val[0][0] * b->val[0][0] + a->val[0][1] * b->val[1][0] + a->val[0][2] * b->val[2][0],
		a->val[0][0] * b->val[0][1] + a->val[0][1] * b->val[1][1] + a->val[0][2] * b->val[2][1],
		a->val[0][0] * b->val[0][2] + a->val[0][1] * b->val[1][2] + a->val[0][2] * b->val[2][2],
	}};
	return out;
}

mat3x3 mat_mult_3x3_3x3(const mat3x3 * a, const mat3x3 * b)
{
	mat3x3 out = {{
		{
			a->val[0][0] * b->val[0][0] + a->val[0][1] * b->val[1][0] + a->val[0][2] * b->val[2][0],
			a->val[0][0] * b->val[0][1] + a->val[0][1] * b->val[1][1] + a->val[0][2] * b->val[2][1],
			a->val[0][0] * b->val[0][2] + a->val[0][1] * b->val[1][2] + a->val[0][2] * b->val[2][2],
		},
		{
			a->val[1][0] * b->val[0][0] + a->val[1][1] * b->val[1][0] + a->val[1][2] * b->val[2][0],
			a->val[1][0] * b->val[0][1] + a->val[1][1] * b->val[1][1] + a->val[1][2] * b->val[2][1],
			a->val[1][0] * b->val[0][2] + a->val[1][1] * b->val[1][2] + a->val[1][2] * b->val[2][2],
		},
		{
			a->val[2][0] * b->val[0][0] + a->val[2][1] * b->val[1][0] + a->val[2][2] * b->val[2][0],
			a->val[2][0] * b->val[0][1] + a->val[2][1] * b->val[1][1] + a->val[2][2] * b->val[2][1],
			a->val[2][0] * b->val[0][2] + a->val[2][1] * b->val[1][2] + a->val[2][2] * b->val[2][2],
		}
	}};
	return out;
}

mat1x3 mat_cross_mult_1x3(const mat1x3 * a, const mat1x3 * b)
{
	mat1x3 out = {{
		{
			a->val[0][1] * b->val[0][2] - a->val[0][2] * b->val[0][1],
			a->val[0][2] * b->val[0][0] - a->val[0][0] * b->val[0][2],
			a->val[0][0] * b->val[0][1] - a->val[0][1] * b->val[0][0]
		}
	}};
	return out;
}

float mat_dot_mult_1x3(const mat1x3 * a, const mat1x3 * b)
{
	return
		a->val[0][0] * b->val[0][0] +
		a->val[0][1] * b->val[0][1] +
		a->val[0][2] * b->val[0][2];
}

mat1x3 mat_add_1x3(const mat1x3 * a, const mat1x3 * b)
{
	mat1x3 out = {{
		{
			a->val[0][0] + b->val[0][0],
			a->val[0][1] + b->val[0][1],
			a->val[0][2] + b->val[0][2],
		}
	}};
	return out;
}

mat1x3 mat_sub_1x3(const mat1x3 * a, const mat1x3 * b)
{
	mat1x3 out = {{
		{
			a->val[0][0] - b->val[0][0],
			a->val[0][1] - b->val[0][1],
			a->val[0][2] - b->val[0][2],
		}
	}};
	return out;
}
