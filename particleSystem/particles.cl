__kernel void compute(__global float3 *data_buffer,
								float mouseX,
								float mouseY)
{
	int gid = get_global_id(0);
	float mass		= data_buffer[3*gid+2].x;
    __global float3* velocity = &data_buffer[3*gid+1];
    __global float3* pos = &data_buffer[3*gid];
	*pos = (float3)(mouseX, mouseY, 0.0f);
}
