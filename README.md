# Упрощенный вектор
Данная структура данных - упрощенный вариант std::vector библиотеки STL. Хранит у себя объект класса ArrayPtr, который является "оберткой" для сырого указателя. 
## Возможности
Функционал __упрощенного вектора (SimpleVector)__:
* при создании вектора можно указать: 
  * количество элементов вектора
  * вместимость вектора 
  * количество элементов и значение, которым их нужно инициализировать (значение может быть lvalue или rvalue)
  * std::initializer_list
  * другой вектор (SimpleVector), lvalue или rvalue

* PushBack(...) - добавляет элемент в конец вектора. При нехватке места увеличивает вдвое вместимость. Работает с lvalue и rvalue
* Insert(...) - вставляет элемент в необходимую позицию, возвращает итератор на вставленное значение. Если перед вставкой значения вектор был заполнен полностью, то вместимость должна увеличиться вдвое (если вместимость = 0, то стать равной 1). Работает с lvalue и rvalue
* PopBack() - "удаляет" последний элемент вектора (уменьшает размер вектора)
* Erase(...) - удаляет элемент в указанной позиции 
* swap(...) - обменивает значение с другим вектором. Работает с lvalue и rvalue
* GetSize() - возвращает количество элементов в векторе  
* GetCapacity() - возвращает вместимость вектора
* IsEmpty() - возвращает true, если вектор пустой
* operator[] - возвращает ссылку на элемент с нужным индексом 
* At(...) - возвращает ссылку на элемент с нужным индексом. Если индекс больше количества элементов в векторе - выбрасывает исключение std::out_of_range
* Clear() - обнуляет размер вектора, не изменяя его вместимость 
* Resize(...) - изменяет размер вектора. При увеличении размера новые элементы получают значение по умолчанию
* Reserve(...) - изменяет вместимость вектора (если вместимость вектора меньше нужной)
* begin() - возвращает итератор на начало вектора 
* end() - возвращает итератор на элемент, следующиий за последним
* cbegin() - возвращает константный итератор на начало вектора 
* cend() - возвращает константный итератор на элемент, следующий за последним 

## Установка
* скопировать файлы array_ptr.h и simple_vector.h в папку с вашим проектом
* добавить как стороннюю библиотеку simple_vector.h