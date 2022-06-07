//
// Created by johnk on 2022/5/22.
//

#include <iostream>

#include <clipp.h>

#include <MetaTool/ClangParser.h>
#include <MetaTool/HeaderGenerator.h>

int main(int argc, char* argv[])
{
    std::string sourceFile;
    std::string outputFile;
    std::vector<std::string> includePaths;

    auto cli = (
        clipp::required("-s").doc("source file to parse") & clipp::value("sourceFile", sourceFile),
        clipp::required("-o").doc("output meta header file") & clipp::value("outputFile", outputFile),
        clipp::repeatable(
            clipp::option("-i") & clipp::value("includePaths", includePaths)
        ).doc("include paths")
    );
    if (!clipp::parse(argc, argv, cli)) {
        std::cout << clipp::make_man_page(cli, argv[0]);
        return -1;
    }

    std::vector<const char*> includePathCharSequences(includePaths.size());
    for (auto i = 0; i < includePathCharSequences.size(); i++) {
        includePathCharSequences[i] = includePaths[i].c_str();
    }

    MetaTool::SourceInfo sourceInfo {};
    sourceInfo.sourceFile = sourceFile.c_str();
    sourceInfo.includePathNum = static_cast<uint32_t>(includePathCharSequences.size());
    sourceInfo.includePaths = includePathCharSequences.data();

    MetaTool::ClangParser clangParser(sourceInfo);
    clangParser.Parse();
    const auto& metaInfo = clangParser.GetMetaContext();

    MetaTool::HeaderGenerator headerGenerator(outputFile.c_str());
    headerGenerator.Generate(metaInfo);

    return 0;
}