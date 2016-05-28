__kernel void gravity(__global float4 *data_buffer,
								float4 grav,
								float mass)
{
	int gid1 = get_global_id(0) * 3;
	int gid2 = get_global_id(1) * 3;
	int gid3 = get_global_id(2) * 3;
	int size1 = get_global_size(0);
	int size2 = get_global_size(1);
    __global float4* velocity = &data_buffer[(gid1 + (gid3 * size1 * size2) + gid2 * size1) + 1];
    __global float4* pos = &data_buffer[(gid1 + gid3 * size1 * size2 + gid2 * size1)];
	float4 vec = grav - *pos;
	float4 force = fast_normalize(vec);
	*velocity +=  force / mass * 0.01f;
	*pos += *velocity * 0.01f;
}

__kernel void init_gravity(__global float4 *data_buffer)
{
	int gid1 = get_global_id(0) * 3;
	int gid2 = get_global_id(1) * 3;
	int gid3 = get_global_id(2) * 3;
	int size1 = get_global_size(0);
	int size2 = get_global_size(1);
	int size3 = get_global_size(2);
    __global float4* velocity = &data_buffer[(gid1 + (gid3 * size1 * size2) + gid2 * size1) + 1];
	*velocity = (float4)(0.1f, 0.1f, 0.1f, 0.0f);
}


__kernel void sphere(__global float4 *data_buffer,
							size_t	total_size)
{
	int gid1 = get_global_id(0);
	int gid2 = get_global_id(1);
	int gid3 = get_global_id(2);
	int size1 = get_global_size(0);
	int size2 = get_global_size(1);
	int size3 = get_global_size(2);
	int index = (gid1 + gid3 * size1 * size2 + gid2 * size1);
	float tmp = (float)(index) / (float)(size1 * size2 * size3);
	__global float4* pos = &data_buffer[index * 3];
	float fst = (float)gid2 / (float)(size1) * (float)M_PI * 4;
	float snd = (float)gid1 / (float)(size2) * (float)M_PI_2 * 4;
	*pos = (float4)(sin(fst) * cos(snd) * (-0.5f + tmp), sin(fst) * sin(snd) * (-0.5f + tmp), -1.0f + (cos(fst) * (-0.5f + tmp)), 0.0f);
}

__kernel void cube(__global float4 *data_buffer,
							size_t	total_size)
{
	int gid1 = get_global_id(0);
	int gid2 = get_global_id(1);
	int gid3 = get_global_id(2);
	int size1 = get_global_size(0);
	int size2 = get_global_size(1);
	int size3 = get_global_size(2);
	int index = (gid1 + gid3 * size1 * size2 + gid2 * size1) * 3;
	__global float4* pos = &data_buffer[index];
	*pos = (float4)(-0.5f + (float)gid1 / (float)size1, -0.5f + (float)gid2 / (float)size2, -1.0f + (float)gid3 / (float)size3, 0.0f);
}
