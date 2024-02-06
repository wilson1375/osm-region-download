#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <cmath>
#include <cstdlib>
#include <cstdio>

static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);
bool downloadTile(double west, double south, double east, double north, const std::string& filename);
void mergeTiles(const std::vector<std::string>& tileFiles, const std::string& outputFilename);
void deleteTileFiles(const std::vector<std::string>& tileFiles);
void parseCommandLine(int argc, char* argv[], double& tileWidth, double& tileHeight, double& north, double& east, double& south, double& west);

int main(int argc, char* argv[])
{
    // Defaults
    double tileWidth = 0.005;
    double tileHeight = 0.003;

    double north = 40.6920;
    double east = -74.0423;
    double south = 40.6881;
    double west = -74.0485;

    parseCommandLine(argc, argv, tileWidth, tileHeight, north, east, south, west);

    std::cout << "Using configuration:" << std::endl;
    std::cout << "Tile Width: " << tileWidth << ", Tile Height: " << tileHeight << std::endl;
    std::cout << "Bounds: N(" << north << ") E(" << east << ") S(" << south << ") W(" << west << ")" << std::endl;

    std::vector<std::string> tileFiles;

    int count = 0;
    for(double y = south; y < north; y += tileHeight)
    {
        for(double x = west; x < east; x += tileWidth)
        {
            std::string filename = "tile_" + std::to_string(count++) + ".osm";
            double tileEast = std::min(x + tileWidth, east);
            double tileNorth = std::min(y + tileHeight, north);

            tileFiles.push_back(filename);
            if(downloadTile(x, y, tileEast, tileNorth, filename))
            {
                std::cout << "Downloaded " << filename << std::endl;
            }
            else
            {
                std::cout << "Failed to download " << filename << std::endl;
            }
        }
    }

    mergeTiles(tileFiles, "merged.osm");
    deleteTileFiles(tileFiles);

    return 0;
}

static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize = size * nmemb;
    return fwrite(contents, size, nmemb, static_cast<FILE*>(userp));
}

bool downloadTile(double west, double south, double east, double north, const std::string& filename)
{
    CURL *curl;
    FILE *fp = NULL;
    CURLcode res;
    std::string url = "https://www.openstreetmap.org/api/0.6/map?bbox=" + 
    std::to_string(west) + "," + 
    std::to_string(south) + "," + 
    std::to_string(east) + "," + 
    std::to_string(north);

    curl = curl_easy_init();

    if(!curl)
    {
        std::cerr << "Curl failed" << std::endl;
        if(fp)
        {
            fclose(fp);
        }

        return false;
    }

    if(curl)
    {
        fp = fopen(filename.c_str(),"wb");

        if(fp == NULL)
        {
            std::cerr << "File cannot be opened" << std::endl;
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        fclose(fp);

        if(res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        return true;
    }

    return false;
}

void mergeTiles(const std::vector<std::string>& tileFiles, const std::string& outputFilename)
{
    std::string command = "osmconvert ";

    for(const auto& tileFile : tileFiles)
    {
        command += tileFile + " ";
    }

    command += "-o=" + outputFilename;

    std::cout << "Merging tiles into: " << outputFilename << std::endl;
    int result = std::system(command.c_str());
    if(result != 0)
    {
        std::cerr << "Failed to merge tiles. Command executed: " << command << std::endl;
    }
}

void deleteTileFiles(const std::vector<std::string>& tileFiles)
{
    for(const auto& tileFile : tileFiles)
    {
        if(remove(tileFile.c_str()) == 0)
        {
            std::cout << "Deleted " << tileFile << std::endl;
        }
        else 
        {
            std::cerr << "Error deleting file " << tileFile << std::endl;
        }
    }
}

void parseCommandLine(int argc, char* argv[], double& tileWidth, double& tileHeight, double& north, double& east, double& south, double& west)
{
    for(int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if(arg == "--width" && i + 1 < argc)
        {
            tileWidth = std::stod(argv[++i]);
        }
        else if(arg == "--height" && i + 1 < argc)
        {
            tileHeight = std::stod(argv[++i]);
        }
        else if((arg == "--north" || arg == "-n") && i + 1 < argc)
        {
            north = std::stod(argv[++i]);
        }
        else if((arg == "--east" || arg == "-e") && i + 1 < argc)
        {
            east = std::stod(argv[++i]);
        }
        else if((arg == "--south" || arg == "-s") && i + 1 < argc)
        {
            south = std::stod(argv[++i]);
        }
        else if((arg == "--west" || arg == "-w") && i + 1 < argc)
        {
            west = std::stod(argv[++i]);
        }
    }
}
