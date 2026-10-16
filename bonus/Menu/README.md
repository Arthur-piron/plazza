How to add a new pizza (bonus/Menu)
==================================

This README explains the minimal steps to add a new pizza type to the bonus `Menu` build and lists the pizzas currently available in `bonus/Menu`.

Files you will typically edit:

- `bonus/Menu/src/Pizza/Pizzas.hpp` — add the new pizza class and set its cooking time and ingredients.
- `bonus/Menu/src/PizzaFactory/PizzaFactory.cpp` — add a `case` to return the new pizza instance.
- `bonus/Menu/src/Pizza/PizzaHandler.cpp` — add the parsing entry (name -> enum) and the string mapping for display.
- `bonus/Menu/src/Pizza/IPizza.hpp` — add a `PizzaType` enum value if your pizza type is entirely new.

Step-by-step
-----------

1. Define the pizza class

   Add a new concrete pizza class in `Pizzas.hpp` (near the other pizza classes). Example:

   ```cpp
   class MyNewPizza : public APizza {
   public:
       MyNewPizza(PizzaSize size) : APizza(PizzaType::MyNewPizza, size) {
           setCookingTime(2500); // base time in milliseconds
           setNeededIngredients(Plazza::ingredients{
               1, // dough
               1, // tomato
               1, // gruyere
               0, // sausage
               0, // ham
               0, // mushroom
               0, // steak
               0, // eggplant
               0, // goatCheese
               0, // chiefLove
               0, // chili
               0, // tofu
               0  // olives
           });
       }
       ~MyNewPizza() override = default;
   };
   ```

2. Add the enum (if needed)

   If `PizzaType::MyNewPizza` does not exist yet, add it to `IPizza.hpp`'s `enum PizzaType`.

3. Register the pizza in the factory

   In `PizzaFactory.cpp` add a `case` that creates and returns your pizza:

   ```cpp
   case MyNewPizza:
       return std::make_shared<MyNewPizza>(size);
   ```

4. Add parsing and display name

   - In `PizzaHandler.cpp` add a mapping entry in the `types` map (lowercase key):
     `{"mynewpizza", MyNewPizza},`

   - In `PizzaHandler::pizzaTypeToString()` add a `case MyNewPizza: return "MyNewPizza";`

5. Rebuild

   From the `bonus/Menu` folder run:

   ```bash
   make -j2
   ```

Available pizzas (bonus/Menu)
-----------------------------

The following pizza types are implemented in `bonus/Menu/src/Pizza/Pizzas.hpp`:

- Margarita — cooking time: 1000 ms
- Regina — cooking time: 2000 ms
- Americana — cooking time: 2000 ms
- Fantasia — cooking time: 4000 ms
- AnneSaucisse — cooking time: 2000 ms
- PJSpecial — cooking time: 3000 ms
- Quentemmental — cooking time: 1500 ms
- Plantoine — cooking time: 2200 ms
- SimHotN — cooking time: 1800 ms
- ChloroFill — cooking time: 2500 ms
- WokNRollDamien — cooking time: 2000 ms
- ValerianAvant — cooking time: 2100 ms
- CyberJayce2077 — cooking time: 3500 ms

Sizes supported (use in orders as shown below): `S`, `M`, `L`, `XL`, `XXL`.

Ordering examples
-----------------

Single pizza:

```text
regina XL x1
```

Multiple pizzas in one order (separate pizzas with `;`):

```text
regina XL x2; fantasia S x1
```
