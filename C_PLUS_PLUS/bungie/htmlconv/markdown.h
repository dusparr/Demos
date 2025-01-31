#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <sstream>

class EscapeRule {
public:
	std::string name;
	std::string target;
	std::string replacement;
	bool ignore;
	EscapeRule(std::string n, std::string t, std::string r, bool i = false) : name(n), target(t), replacement(r), ignore(i) {}
};

class RegexRule {
public:
	std::string name;
	std::regex pattern;
	std::string replacement;
	RegexRule(std::string n, std::regex p, std::string r) : name(n), pattern(p), replacement(r) {}
}; 
class MarkdownConverter {
private:
	std::vector<EscapeRule> escapeReplacements;
	std::vector<EscapeRule> escapeUndo;
	std::vector<RegexRule> regexReplacements;

public:
	MarkdownConverter() {
		escapeReplacements = {
			{"Ampersand", "&", "&amp;"},
			{"Less Than", "<", "&lt;"},
			{"Greater Than", ">", "&gt;"}
		};

		escapeUndo = {
			{"Copyright", "&amp;copy;", "&copy;"} // Allow HTML entities to remain
		};

		regexReplacements = {
			{"Bold", std::regex("\\*\\*([^\\*]+)\\*\\*|__([^_]+)__"), "<strong>$1$2</strong>"},
			{"Italic", std::regex("\\*([^\\*]+)\\*|_([^_]+)_"), "<em>$1$2</em>"},
			{"Bold Italic", std::regex("\\*\\*\\*([^\\*]+)\\*\\*\\*|___([^_]+)___"), "<strong><em>$1$2</em></strong>"},
			{"H6", std::regex("^(#{6})\\s*(.+?)\\s*(#*)$", std::regex_constants::ECMAScript), "<h6>$2</h6>"},
			{"H5", std::regex("^(#{5})\\s*(.+?)\\s*(#*)$", std::regex_constants::ECMAScript), "<h5>$2</h5>"},
			{"H4", std::regex("^(#{4})\\s*(.+?)\\s*(#*)$", std::regex_constants::ECMAScript), "<h4>$2</h4>"},
			{"H3", std::regex("^(#{3})\\s*(.+?)\\s*(#*)$", std::regex_constants::ECMAScript), "<h3>$2</h3>"},
			{"H2", std::regex("^(#{2})\\s*(.+?)\\s*(#*)$", std::regex_constants::ECMAScript), "<h2>$2</h2>"},
			{"H1", std::regex("^(#{1})\\s*(.+?)\\s*(#*)$", std::regex_constants::ECMAScript),  "<h1>$2</h1>"}
		};
	};
	std::string convert(const std::string& markdown);
	std::string processLists(const std::string& markdown);
	static void markdown_to_html(const char* input_markdown_file_path, const char* output_markdown_file_path);
	static void MarkdownToHtml(std::string inputMarkdownFilePath, std::string outputMarkdownFilePath);
};


#pragma once