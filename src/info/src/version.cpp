#include <sstream>
#include <info/version.h>

std::string_view info::info_project_full() {
  std::stringstream ss;
  ss << "author: " << info_project_author() << std::endl;
  ss << "url: " << info_project_repository_url() << std::endl;
  ss << "branch: " << info_project_repository_branch() << std::endl;
  ss << "rev: " << info_project_revision() << std::endl;
  ss << "rev date: " << info_project_revision_datetime() << std::endl;
  ss << "rev hist: " << info_project_revision_history() << std::endl;
  return ss.str();
}
