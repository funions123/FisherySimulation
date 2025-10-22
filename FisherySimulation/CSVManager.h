#pragma once

#include <string>
#include <fstream>
#include <iomanip>

/**
 * @class CSVManager
 * @brief A simple helper class to create and write data to a CSV file.
 */
class CSVManager {
public:
    /**
     * @brief Default constructor.
     */
    CSVManager() {};

    /**
     * @brief Opens a new CSV file for writing.
     * @param filename The name of the file to create (e.g., "output.csv").
     * @return True if the file was opened successfully, false otherwise.
     */
    bool open(const std::string& filename)
    {
        if (fileStream.is_open())
        {
            fileStream.close();
        }

        fileStream.open(filename, std::ios::out | std::ios::trunc);
        if (!fileStream.is_open())
        {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return false;
        }
        fileStream << std::fixed << std::setprecision(8);
        return true;
    }

    /**
     * @brief Closes the currently open file stream.
     */
    void close()
    {
        if (fileStream.is_open())
        {
            fileStream.close();
        }
    }

    /**
     * @brief Writes a single string as a header line to the CSV file.
     * @param header The header string (e.g., "Year,Value1,Value2").
     */
    void writeHeader(const std::string& header)
    {
        if (fileStream.is_open())
        {
            fileStream << header << "\n";
        }
    }

    /**
     * @brief Writes a row of data for the Simple Model.
     */
    void writeRow(int year, double fishStock)
    {
        if (fileStream.is_open())
        {
            fileStream << year << separator
                << fishStock << "\n";
        }
    }

    /**
     * @brief Writes a row of data for the Delay Equation Model.
     */
    void writeRow(double time, double n, double E, double S)
    {
        if (fileStream.is_open())
        {
            fileStream << time << separator
                << n << separator
                << E << separator
                << S << "\n";
        }
    }

private:
    std::ofstream fileStream;
    std::string separator = ",";

};