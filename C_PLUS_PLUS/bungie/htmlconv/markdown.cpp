#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <sstream>
#include "markdown.h"
//class EscapeRule {
//public:
//	std::string name;
//	std::string target;
//	std::string replacement;
//	bool ignore;
//	EscapeRule(std::string n, std::string t, std::string r, bool i = false) : name(n), target(t), replacement(r), ignore(i) {}
//};
//
//class RegexRule {
//public:
//	std::string name;
//	std::regex pattern;
//	std::string replacement;
//	RegexRule(std::string n, std::regex p, std::string r) : name(n), pattern(p), replacement(r) {}
//};



std::string MarkdownConverter::convert(const std::string& markdown) {
	std::string html = markdown;

	for (const auto& rule : escapeReplacements) {
		size_t pos = 0;
		while ((pos = html.find(rule.target, pos)) != std::string::npos) {
			html.replace(pos, rule.target.length(), rule.replacement);
			pos += rule.replacement.length();
		}
	}
	for (const auto& rule : regexReplacements) {
		html = std::regex_replace(html, rule.pattern, rule.replacement);
	}


	html = processLists(html);


	for (const auto& rule : escapeUndo) {
		size_t pos = 0;
		while ((pos = html.find(rule.target, pos)) != std::string::npos) {
			html.replace(pos, rule.target.length(), rule.replacement);
			pos += rule.replacement.length();
		}
	}

	return html;
}

std::string MarkdownConverter::processLists(const std::string& markdown) {
	std::istringstream stream(markdown);
	std::ostringstream result;
	std::string line;
	bool inUl = false, inOl = false;
	bool inParagraph = false, startedParagraph = false;

	while (std::getline(stream, line)) {
		if (!inParagraph &&
			!(
				(line.size() >= 2 && std::regex_match(line, std::regex("^\\d+\\. .+"))) ||
				(line.size() >= 2 && line[0] == '<' && line[1] == 'h') ||
				(line.size() >= 2 && line[0] == '-' || line[0] == '+' || line[0] == '*'))
			)
		{
			inParagraph = true;
		}
		if (!inParagraph) // iin list
		{
			if (std::regex_match(line, std::regex("^[-+*] .+"))) {
				if (!inUl) {
					result << "<ul>\n";
					inUl = true;
				}
				result << "<li>" << line.substr(2) << "</li>\n";
			}
			else if (std::regex_match(line, std::regex("^\\d+\\. .+"))) {
				if (!inOl) {
					result << "<ol>\n";
					inOl = true;
				}
				result << "<li>" << line.substr(line.find(' ') + 1) << "</li>\n";
			}
			else {
				if (inUl) {
					result << "</ul>\n";
					inUl = false;
				}
				if (inOl) {
					result << "</ol>\n";
					inOl = false;
				}
				result << line << "\n";
			}
		}
		else //in paragraph
		{
			if (!line.empty())
			{
				if (!startedParagraph)
				{
					result << "<p>" << line;
					startedParagraph = true;
				}
				else
				{
					result << "\n" << line;
				}
			}
			else
			{
				if (startedParagraph)
				{
					result << "</p>" << line;
				}
				else
				{
					result << line;
				}

				if (inUl) {
					result << "</ul>\n";
					inUl = false;
				}
				if (inOl) {
					result << "</ol>\n";
					inOl = false;
				}
				if (startedParagraph)
				{
					result << "\n";
				}
				inParagraph = false;
				startedParagraph = false;
			}
		}
	}
	if (inParagraph) result << "</p>";
	if (inUl) result << "</ul>";
	if (inOl) result << "</ol>";
	return result.str();
}
void MarkdownConverter::MarkdownToHtml(std::string inputMarkdownFilePath, std::string outputMarkdownFilePath) {
	std::ifstream inputFile(inputMarkdownFilePath);
	if (!inputFile) {
		std::cerr << "Error opening input file!\n";
		return;
	}

	std::ofstream outputFile(outputMarkdownFilePath);
	if (!outputFile) {
		std::cerr << "Error opening output file!\n";
		return;
	}

	std::string markdown((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
	MarkdownConverter converter;
	std::string html = converter.convert(markdown);

	outputFile << html;
}

void MarkdownConverter::markdown_to_html(const char* input_markdown_file_path, const char* output_markdown_file_path) {
	MarkdownToHtml(std::string(input_markdown_file_path), std::string(output_markdown_file_path));
}


