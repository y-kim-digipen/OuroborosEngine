#include "buffer.h"

void Renderer::UniformBuffer::AddMember(const std::string& name, DataType data_type, uint32_t size, uint32_t offset)
{
	if (member_vars.find(name) == member_vars.end()) {
		member_vars[name].name = name;
		member_vars[name].type = data_type;
		member_vars[name].size = size;
		member_vars[name].offset = offset;
	}
}
