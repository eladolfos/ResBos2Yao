#ifndef HISTOGRAM_TEST_HH
#define HISTOGRAM_TEST_HH

class MockHistogram {
    public:
        MOCK_METHOD2(Fill,void(const double&, const double&));
        MOCK_METHOD1(Scale,void(const double&));
        MOCK_METHOD1(Normalize,void(const double&));
        MOCK_CONST_METHOD0(Integral,double());
        MOCK_CONST_METHOD2(Integral,double(const size_t&, const size_t&));
        MOCK_CONST_METHOD0(Save,void());
        MOCK_CONST_METHOD1(Save,void(const std::string&));
};

#endif
