#include "ConfigMgr.h"

ConfigMgr::ConfigMgr()
{
	boost::filesystem::path current_path = boost::filesystem::current_path();	// ��ǰ�ļ�·��
	boost::filesystem::path config_path = current_path / "config.ini";			// ·��ƴ�ӵõ�config.ini�ļ���·��
	std::cout << "Config path : " << config_path << std::endl;

	boost::property_tree::ptree ptree;
	boost::property_tree::read_ini(config_path.string(), ptree);

	for (const auto& section_pair : ptree) {
		const ::std::string& section_name = section_pair.first;
		const boost::property_tree::ptree& section_tree = section_pair.second;
		std::map<std::string, std::string> section_config;
		for (const auto& key_value_pair : section_tree) {
			const std::string& key = key_value_pair.first;
			const std::string& value = key_value_pair.second.get_value<std::string>();
			section_config[key] = value;
		}

		SectionInfo sectionInfo;
		sectionInfo._section_datas = section_config;
		// ��section��key-value�Ա��浽config_map��
		_config_map[section_name] = sectionInfo;
	}

	// ������е�section��key-value��  
	for (const auto& section_entry : _config_map) {
		const std::string& section_name = section_entry.first;
		SectionInfo section_config = section_entry.second;
		std::cout << "[" << section_name << "]" << std::endl;
		for (const auto& key_value_pair : section_config._section_datas) {
			std::cout << key_value_pair.first << "=" << key_value_pair.second << std::endl;
		}
	}
}
