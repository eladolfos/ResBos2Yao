#include "ResBos/HashGrid.hh"

#include "Hash_test.hh"

TEST_F(HashTest, ResummationGridCSS) {
    ON_CALL(*settings, GetSettingString("XSec"))
        .WillByDefault(Return("Resummation"));
    ON_CALL(*settings, GetSettingString("scheme"))
        .WillByDefault(Return("CSS"))
    EXPECT_CALL(*settings, GetBOrder())
        .WillOnce(Return(0))
        .WillOnce(Return(1))
        .WillOnce(Return(2))
        .WillRepeatedly(Return(3));
    EXPECT_CALL(*settings, GetCOrder())
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(1))
        .WillRepeatedly(Return(2));
    EXPECT_CALL(*settings, GetScheme())
        .WillRepeatedly(Return(Scheme::CSS));
    EXPECT_CALL(*settings, GetC1())
        .WillRepeatedly(Return(QCD::B0));
    EXPECT_CALL(*settings, GetC2())
        .WillRepeatedly(Return(1.0));
    EXPECT_CALL(*settings, GetC3())
        .WillRepeatedly(Return(QCD::B0));
    EXPECT_CALL(*settings, GetNonPertEnum())
        .WillRepeatedly(Return(NonPertEnum::BLNY));
    EXPECT_CALL(*settings, GetNonPert())
        .WillRepeatedly(Return(std::vector<double>{0.2, 0.3, 0.4}));

    size_t hash1 = std::hash<IO::Settings>{}(*settings);
    size_t hash2 = std::hash<IO::Settings>{}(*settings);
    size_t hash3 = std::hash<IO::Settings>{}(*settings);
    size_t hash4 = std::hash<IO::Settings>{}(*settings);

    std::cout << hash1 << " " << hash2 << " " << hash3 << " " << hash4 << std::endl;

    EXPECT_NE(hash1, hash2);
    EXPECT_NE(hash1, hash3);
    EXPECT_NE(hash1, hash4);
    EXPECT_NE(hash2, hash3);
    EXPECT_NE(hash2, hash4);
    EXPECT_NE(hash3, hash4);
}
