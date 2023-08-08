#pragma once
#include <string_view>
#include "config.h"
namespace info {

constexpr std::string_view info_project_author(){ return PROJECT_AUTHOR; }
constexpr std::string_view info_project_repository_url() { return PROJECT_REPOSITORY_URL; }
constexpr std::string_view info_project_repository_branch() { return PROJECT_REPOSITORY_BRANCH; }
constexpr std::string_view info_project_revision() { return PROJECT_REVISION; }
constexpr std::string_view info_project_revision_datetime() { return PROJECT_REVISION_DATETIME; }
constexpr std::string_view info_project_revision_history() { return PROJECT_REVISION_HISTORY; }
constexpr std::string_view info_project_version() { return PROJECT_VERSION; }

std::string_view info_project_full();

} // namespace info