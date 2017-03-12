#include "measurable_test_utils.hpp"


namespace btree
{
    std::vector<int> input_from_file(std::string filename)
    {
        std::vector<int> result;
        std::ifstream file;
        file.open(filename);
        std::string line;
        while(getline(file, line))
        {
            result.push_back(std::stoi(line));
        }

        file.close();

        return result;
    }

    void test_from_file(std::string filename)
    {
        std::vector<int> inputs = input_from_file(filename);

        MeasurableBtree<2> t;
        for (int i : inputs)
        {
            t.add(i, "hello");
        }

        std::sort(inputs.begin(), inputs.end());
        auto result = t.dump();
        for (size_t i = 0; i < inputs.size(); i++)
        {
            ASSERT_EQ(inputs[i], result[i]);
        }

        ASSERT_EQ(Measurable::shallowest, Measurable::deepest);
    }
}
