#include <RmlUi/CApi/Log.h>
#include <RmlUi/Core/Log.h>
#include <vector>

void rml_log_message(RmlLogType type, RmlStringView message)
{
	std::vector<char> buffer;
	if (message.data && message.size > 0)
	{
		buffer.assign(message.data, message.data + message.size);
		buffer.push_back('\0');
	}
	else
	{
		buffer.push_back('\0');
	}

	Rml::Log::Message(static_cast<Rml::Log::Type>(type), "%s", buffer.data());
}