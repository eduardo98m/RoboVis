//#pragma once

namespace rbvs
{

    /**
     * @brief Generates a globally unique component type ID.
     *
     * This inline function uses a static counter to ensure that each call returns a unique integer.
     *
     * @return int A unique component type ID.
     */
    inline int getNextComponentTypeId()
    {
        static int nextId = 0;
        return nextId++;
    }

    /**
     * @brief Template for generating a unique type ID for each component type.
     *
     * The ComponentType struct template provides a static method to retrieve a unique integer ID
     * for a given component type T. The ID is assigned the first time the method is called.
     *
     * @tparam T The component type.
     */
    template <typename T>
    struct ComponentType
    {
        /**
         * @brief Returns a unique component type ID.
         *
         * The first time this method is called for a particular type T, a new ID is generated using
         * the global counter. Subsequent calls return the cached value.
         *
         * @return int Unique ID for the component type.
         */
        static int id()
        {
            static int typeId = getNextComponentTypeId();
            return typeId;
        }
    };

} // namespace rbvs
