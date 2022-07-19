#include "buffer.h"

namespace Renderer {

	void UniformBuffer::ShutDown()
	{
		if (data != nullptr)
			free(data);
		data = nullptr;
	}

	void UniformBuffer::AddMember(const std::string& name, DataType data_type, uint32_t size, uint32_t offset)
	{
		if (member_vars.find(name) == member_vars.end()) {
			member_vars[name].name = name;
			member_vars[name].type = data_type;
			member_vars[name].size = size;
			member_vars[name].offset = offset;
		}
	}

	int UniformBuffer::UpdateData(const char* member_var_name, void* data)
	{
		if (member_vars.find(member_var_name) != member_vars.end())
		{
			const UniformBufferMember& ub_mem = member_vars[member_var_name];
			memcpy_s((char*)this->data + ub_mem.offset, ub_mem.size, data, ub_mem.size);

			return 0;
		}

		return -1;
	}

}
