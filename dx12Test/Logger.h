#pragma once

struct LoggerConfiguration
{
	std::string rotating_filename = "logs/default.log";
	size_t rotating_max_file_size = 1024 * 1024 * 10;
	size_t rotating_max_files = 1;
};

class Logger
{
public:
	static void Configure(const LoggerConfiguration& cfg);

	Logger(const std::string& name);
	Logger(const std::string& name, const LoggerConfiguration& cfg);

	template<class T>
	inline void Trace(const T& msg) const { m_logger->trace(msg); };

	template<class... TArgs>
	inline void Trace(const std::string& fmtStr, TArgs&&... args) const {
		m_logger->trace(fmt::runtime(fmtStr), std::forward<TArgs>(args)...);
	};

	template<class T>
	inline void Debug(const T& msg) const { m_logger->debug(msg); };

	template<class... TArgs>
	inline void Debug(const std::string& fmtStr, TArgs&&... args) const {
		m_logger->debug(fmt::runtime(fmtStr), std::forward<TArgs>(args)...);
	};

	template<class T>
	inline void Info(const T& msg) const { m_logger->info(msg); };

	template<class... TArgs>
	inline void Info(const std::string& fmtStr, TArgs&&... args) const { 
		m_logger->info(fmt::runtime(fmtStr), std::forward<TArgs>(args)...);
	};

	template<class T>
	inline void Warn(const T& msg) const { m_logger->warn(msg); };

	template<class... TArgs>
	inline void Warn(const std::string& fmtStr, TArgs&&... args) const {
		m_logger->warn(fmt::runtime(fmtStr), std::forward<TArgs>(args)...);
	};

	template<class T>
	inline void Error(const T& msg) const { m_logger->error(msg); };

	template<class... TArgs>
	inline void Error(const std::string& fmtStr, TArgs&&... args) const {
		m_logger->error(fmt::runtime(fmtStr), std::forward<TArgs>(args)...);
	};

	template<class T>
	inline void Critical(const T& msg) const { m_logger->critical(msg); };

	template<class... TArgs>
	inline void Critical(const std::string& fmtStr, TArgs&&... args) const {
		m_logger->critical(fmt::runtime(fmtStr), std::forward<TArgs>(args)...);
	};

	void Flush();
private:
	std::shared_ptr<spdlog::logger> m_logger;
	static std::optional<LoggerConfiguration> configuration;
};

