#pragma once
namespace aos {

/**
 * @brief Interface for calculating the minimum allowed quantity for an order
 * given a specific price.
 *
 * This interface is intended to be implemented by classes that define
 * exchange-specific or strategy-specific rules for computing the minimum
 * acceptable quantity based on price. It is typically used to enforce notional
 * filters (e.g., minimum notional value).
 *
 * @tparam Price The type used to represent price (e.g., double, Decimal).
 * @tparam Qty The type used to represent quantity (e.g., double, int).
 */
template <typename Price, typename Qty>
class OrderMinQtyCalculatorInterface {
  public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~OrderMinQtyCalculatorInterface()           = default;

    /**
     * @brief Calculates the minimum quantity required at the given price.
     *
     * This method should return the smallest quantity allowed for an order
     * at the specified price, typically based on notional constraints such as
     * minimum order value (price * quantity).
     *
     * If the price is aligned with the price step, the method calculates and
     * returns the required quantity in the result with status `true`.
     * Otherwise, if the price is not properly aligned, the method returns
     * status `false` and an undefined quantity.
     *
     * @param price The price at which the order would be placed.
     * @return A pair where the first value indicates success (true if price is
     * aligned), and the second is the minimum allowed quantity at the given
     * price.
     */
    virtual std::pair<bool, Qty> Calculate(Price price) = 0;
};

}  // namespace aos
