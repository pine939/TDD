#include <iostream>
#include <gtest/gtest.h>
class Money;
class Bank;
class Expression {
public:
    virtual Money *reduce(Bank *bank, std::string to) = 0;
    virtual int GetAmount() {}
    virtual Expression *plus(Expression *addend) = 0;
    virtual Expression *times(int multiple) = 0;
};
class Money : public Expression {
protected:
    int amount_;
    std::string currency_;
public:
    Money(int amount, std::string currency) {
        amount_ = amount;
        currency_ = currency;
    }
    int GetAmount() override {
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
    Expression *times(int multiple) override {
        return new Money(amount_ * multiple, currency_);
    }
    Expression *plus(Expression *addend) override ;
    Money *reduce(Bank *bank, std::string to);
};
class Bank {
public:
    Money *reduce(Expression *source, std::string to);
    void addRate(std::string from, std::string to, int rate);
    int rate(std::string from, std::string to);
private:
    std::map<std::pair<std::string, std::string>, int> rates;
};

class Sum : public Expression {
public:
    Expression *augend_;
    Expression *addend_;
    Sum (Expression *augend, Expression *addend) {
        augend_ = augend;
        addend_ = addend;
    }
    Money *reduce(Bank *bank, std::string to);
    Expression *plus(Expression *addend) override;
    Expression *times(int multiple) override {
        return new Sum(augend_->times(multiple), addend_->times(multiple));
    }
};

Money *Sum::reduce(Bank *bank, std::string to) {
    int amount = augend_->reduce(bank, to)->GetAmount() + addend_->reduce(bank, to)->GetAmount();
    return new Money(amount, to);
}

Expression *Sum::plus(Expression *addend) {
    return new Sum(this, addend);
}

Money *Bank::reduce(Expression *source, std::string to) {
    return source->reduce(this, to);
}

void Bank::addRate(std::string from, std::string to, int rate) {
    rates[std::make_pair(from, to)] = rate;
}

int Bank::rate(std::string from, std::string to) {
    if (from.compare(to) == 0) return 1;
    int rate = rates[std::make_pair(from, to)];
    return rate;
}

Money *Money::dollar(int amount) {
    return new Money(amount, "USD");
}

Money *Money::franc(int amount) {
    return new Money(amount, "CHF");
}

Expression *Money::plus(Expression *addend) {
    return new Sum(this, addend);
}

Money *Money::reduce(Bank *bank, std::string to) {
    int rate = bank->rate(currency_, to);
    return new Money(amount_/rate, to);
}

TEST(MoneyTestSuit, temp_addition)
{
    Money *five = Money::dollar(5);
    Bank *bank = new Bank();
    Expression *sum = five->plus(five);
    Money *reduced = bank->reduce(sum, "USD");
    ASSERT_EQ(Money::dollar(10)->GetAmount(), reduced->GetAmount());
}

TEST(MoneyTestSuit, plus_return_sum)
{
    Money *three = Money::dollar(3);
    Money *four = Money::dollar(4);
    Expression *result = three->plus(four);
    Sum *sum = (Sum *)result;
    ASSERT_EQ(three->GetAmount(), sum->augend_->GetAmount());
    ASSERT_EQ(four->GetAmount(), sum->addend_->GetAmount());
}
TEST(MoneyTestSuit, reduce_sum)
{
    Expression *sum = new Sum(Money::dollar(3), Money::dollar(4));
    Bank *bank = new Bank();
    Money *result = bank->reduce(sum, "USD");
    ASSERT_EQ(Money::dollar(7)->GetAmount(), result->GetAmount());
}
TEST(MoneyTestSuit, reduce_money)
{
    Bank *bank = new Bank();
    Money *result = bank->reduce(Money::dollar(1), "USD");
    ASSERT_EQ(Money::dollar(1)->GetAmount(), result->GetAmount());
}

TEST(MoneyTestSuit, bank_addrate)
{
    Bank *bank = new Bank();
    bank->addRate("CHF", "USD", 2);
    Money *result = bank->reduce(Money::franc(2), "USD");
    ASSERT_EQ(Money::dollar(1)->GetAmount(), result->GetAmount());
}

TEST(MoneyTestSuit, mixed_addition)
{
    Expression *fiveBucks = Money::dollar(5);
    Expression *tenFranc = Money::franc(10);
    Bank *bank = new Bank();
    bank->addRate("CHF", "USD", 2);

    Money *result = bank->reduce(fiveBucks->plus(tenFranc), "USD");
    ASSERT_EQ(Money::dollar(10)->GetAmount(), result->GetAmount());
}

TEST(MoneyTestSuit, sum_plus)
{
    Expression *fiveBucks = Money::dollar(5);
    Expression *tenFrancs = Money::franc(10);
    Bank *bank = new Bank();
    bank->addRate("CHF", "USD", 2);
    Expression *sum = (new Sum(fiveBucks, tenFrancs))->plus(fiveBucks);

    Money *result = bank->reduce(sum, "USD");
    ASSERT_EQ(Money::dollar(15)->GetAmount(), result->GetAmount());
}

TEST(MoneyTestSuit, sum_times)
{
    Expression *fiveBucks = Money::dollar(5);
    Expression *tenFrancs = Money::franc(10);
    Bank *bank = new Bank();
    bank->addRate("CHF", "USD", 2);
    Expression *sum = (new Sum(fiveBucks, tenFrancs))->times(2);

    Money *result = bank->reduce(sum, "USD");
    ASSERT_EQ(Money::dollar(20)->GetAmount(), result->GetAmount());
}

TEST(MoneyTestSuit, multiple)
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