#include "pch.h"
#include "Logger.h"

using namespace spdlog;

std::optional<LoggerConfiguration> Logger::configuration = std::nullopt;

void Logger::Configure(const LoggerConfiguration& cfg)
{
	configuration = cfg;
}

Logger::Logger(const std::string& name)
{
	LoggerConfiguration cfg;

	if (configuration.has_value())
		cfg = configuration.value();

	std::vector<sink_ptr> sinks;
	sinks.push_back(std::make_shared<sinks::rotating_file_sink_mt>(
		cfg.rotating_filename,
		cfg.rotating_max_file_size,
		cfg.rotating_max_files)
	);
	sinks.push_back(std::make_shared<sinks::msvc_sink_mt>());

	m_logger = std::make_shared<logger>(name, sinks.begin(), sinks.end());
	m_logger->flush_on(level::critical);
	m_logger->flush_on(level::err);
	m_logger->flush_on(level::trace);
}

Logger::Logger(const std::string& name, const LoggerConfiguration& cfg)
{
	std::vector<sink_ptr> sinks;
	sinks.push_back(std::make_shared<sinks::rotating_file_sink_mt>(
		cfg.rotating_filename,
		cfg.rotating_max_file_size,
		cfg.rotating_max_files)
	);
	sinks.push_back(std::make_shared<sinks::msvc_sink_mt>());

	m_logger = std::make_shared<logger>(name, sinks.begin(), sinks.end());
	m_logger->flush_on(level::critical);
	m_logger->flush_on(level::err);
	m_logger->flush_on(level::trace);
}

void Logger::Flush()
{
	m_logger->flush();
}
