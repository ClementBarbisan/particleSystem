__kernel void compute(__global float4 *data_buffer,
								float mouseX,
								float mouseY)
{
	int gid1 = get_global_id(0) * 3;
	int gid2 = get_global_id(1) * 3;
	int gid3 = get_global_id(2) * 3;
	int size1 = get_global_size(0);
	int size2 = get_global_size(1);
	float mass		= data_buffer[(gid1 + (gid3 * size1 * size2) + gid2 * size1) + 2].x;
    __global float4* velocity = &data_buffer[(gid1 + (gid3 * size1 * size2) + gid2 * size1) + 1];
    __global float4* pos = &data_buffer[(gid1 + gid3 * size1 * size2 + gid2 * size1)];
	*pos -= (float4)(0.01f, 0.01f, 0.01f, 0.0f);
}
