#include <iostream>
#include <gtest/gtest.h>

class Money  {
protected:
    int amount_;
    std::string currency_;
public:
    Money(int amount, std::string currency) {
        amount_ = amount;
        currency_ = currency;
    }
    int GetAmount() {
        return amount_;
    }
    std::string GetCurrency() {
        return currency_;
    }
    bool equals(void *obj) {
        Money *money = (Money *)obj;
        return (amount_ == money->GetAmount()) && (currency_ == money->currency_);
    }
    static Money *dollar(int amount);
    static Money *franc(int amount);
    Money *times(int multiple) {
        return new Money(amount_ * multiple, currency_);
    }

};
Money *Money::dollar(int amount) {
    return new Money(amount, "USD");
}

Money *Money::franc(int amount) {
    return new Money(amount, "CHF");
}

TEST(DollarTestSuit, multiple)
{
    Money *dollar = Money::dollar(5);
    ASSERT_EQ((Money::dollar(10)->GetAmount()), (dollar->times(2))->GetAmount());
    ASSERT_EQ((Money::dollar(15))->GetAmount(), (dollar->times(3))->GetAmount());

    Money *franc = Money::franc(5);
    ASSERT_EQ((Money::franc(10))->GetAmount(), (franc->times(2))->GetAmount());
    ASSERT_EQ((Money::franc(15))->GetAmount(), (franc->times(3))->GetAmount());
}
TEST(MoneyTestSuit, equality)
{
    ASSERT_TRUE((Money::dollar(5))->equals(Money::dollar(5)));
    ASSERT_FALSE((Money::dollar(5))->equals(Money::dollar(6)));
    ASSERT_TRUE((Money::franc(5))->equals(Money::franc(5)));
    ASSERT_FALSE((Money::franc(5))->equals(Money::franc(6)));

    ASSERT_FALSE((Money::franc(5))->equals(Money::dollar(5)));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}