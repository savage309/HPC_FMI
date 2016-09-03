Задачата от тази седмица е с по-голямо тегло от останалите.
Очакваме решенията да използват Data Oriented Design.
Този път ще ги проверяваме НА РЪКА.

---

# Симулатор на вселена


Напишете симулатор на вселена, работещ на принципа за решаване на [N-Body problem](https://en.wikipedia.org/wiki/N-body_problem).

---

Симулаторът се състой от следните функции:

    void Universe_Initialize(const char* file)
    {
        // чете описанието на вселената от файла file
    }

    void Universe_Run(float time, float delta, Result* result)
    {
      // magic
    }

Функцията `Universe_Run` симулира вселената до момента `time` в секунди, с дискретни стъпки от по `delta` секунди. На всяка стъпка от момент 0 до момент `time`, `Universe_Run` смята новите позиция и скорост на всяко тяло и намира дали някои тела не са се сблъскали или някое тяло не е било унищожено от *Death Star* или *X-Wing*.

---

За да намерите новата позиция на дадено тяло *х*, ви трябва следните физични закони:


- `P = v*t + (a*t*t) / 2`, пътят който тялото изминава за време *t* при движение със скорот *v* и ускорение *a*
- `a = F/m`, *F* силата която действа на едно тяло, *m* - масата на тялото
- `r_ij` e вектора между тела *i* и *j*
- `n_ij` e нормализирания вектор между тела *i* и *j*
- `F_ij = ((G * m_i * m_j) /  (r_ij * r_ij)) * n_ij`, където:
  - *m_i* е масата на тяло *i*
  - *G* е [гравитационната константа](https://en.wikipedia.org/wiki/Gravitational_constant)
- `F_i = sum(j != i, F_ij)` - е силата, която действа на дадено тяло

---

На http://mbostock.github.io/protovis/ex/nbody.html можете да видите проста визуализация на алгоритъма.

Вселената е описана в следния формат:

    N
    <id> <Type> <Mass> <Size> <Position> <Speed> [<Acceleration> <Fuel> <Consumption> <Range>]
    ...
    <id> <Type> <Mass> <Size> <Position> <Speed> [<Acceleration> <Fuel> <Consumption> <Range>]

---
	
Където:

  - *id* - уникален идентификатор на тялото
    - `int`
  - *N* е броя на телата във вселената
  - *Type* е типът на тялото, едно от:
    - *Planet*
    - *Asteroid*
    - *Death Star*
    - *X-Wing*
  - *Mass* - масата на тялото в
  - *Size* - размер на тялото
    - `float`
  - *Position* - началната позиция на тялото
    - точка в три-измерното пространство записана като `X Y Z`
  - *Speed* - началната скорост на тялото
    - вектор в три-измерното пространство записан като `X Y Z`
  - *Acceleration* - постоянно ускорение генерирано от двигателя на *Death Star* или *X-Wing*
    - вектор в три-измерното пространство записан като `X Y Z`
  - *Fuel* - количеството гориво за двигателя на *Death Star* или *X-Wing*
    - `float` литри
  - *Consumption* - разход в литри за секунда на двигателя на *Death Star* или *X-Wing*
    - `float` *l/s*
  - *Range* - обхват на оръжията на  *Death Star* или *X-Wing*
    - `float`

---

Правила:
    - Всички разстояния и координати са в километри *km*
    - Ако *Planet* или *Asteroid* попадне в обхвата на *Death Star*, те биват унищожени
    - Ако *Asteroid* или *Death Start* попадне в обхвата на *X-Wing*, те биват унищожени
    - Ако две тела се сблъскат - и двете се унищожават

## Резултат

Функцията `Universe_Run` трябва да попълни структура от данни `Result`:

    struct Vector3D
    {
        float x, y, z;
    };

    struct Destruction
    {
        float time; // timestamp of the destruction
        int destructor; // id of the Death Star or X-Wing
        int destructed; // id of the destroyed body
    };

    struct Collision
    {
      float time; // timestamp of the collision
      int body1; // id of the first collided body
      int body2; // id of the second collided body
    }

    struct BodyPosition
    {
      int body;
      Vector3D position;
    }

    struct Result
    {
        Destruction* destructions;
        Collision* collisions;
        BodyPosition* positions;

        int destructions_count;
        int collisions_count;
        int positions_count;
    };

---

!! Паметта за `destructions`, `collistions`, `positions` ще бъде предварително заделена.

---

Решенията изпращайте на blagovestt ет uni-sofia точка bg

Заглавие `HPC FMI 44286 HOMEWORK 3`, където 44286 е факултетният ви номер

Съдържание `ТРИТЕ ВИ ИМЕНА` 

и прикачен файл `main.cpp`, в който е решението на задачата

---

Можете да ползвате STL, но не можете да ползвате други(трети) библиотеки. 
Приемат се решения писани на C/C++.
Компилаторите, с които тестваме поддържат `С++14`. 

Multithreading не е позволен. Можете да ползвате всички налични към настоящия момент и общи за AMD и Intel разширения - MMX, SSE, SSE2, SSE3, SSSE3, SSE4.1, SSE4.2 и AVX.
Hint: постарайте се да използват някои от тези разширения при решаването на задачите.
Ще тестваме на процесор `Intel Core i7 (I7-3720QM)`.

---

Решенията трябва да са крос-платформени. Ако използвате специфични извиквания (OS || compiler dependent) те трябва да са написани така, че кодът да се компилира и работи на Windows, Linux & OS X с Visual Studio 2015 и clang 3.6+ съответно (тоест, с ifdef).

---

Можете да изпращате решения многократно, като само последното ще бъде разгледано.

---

**Ако имате въпроси: пишете на trendafilov.dn ет gmail точка com**

---

Краен срок : 01.ХII.2015 (Вторник), 17:59.

---

Имайте предвид, че за да постигнем по-голяма прозрачност при оценяването, кода който изпращате може да бъде публикуван на място с публичен достъп. 
Скрипта, с който тестваме решенията Ви се намира тук https://github.com/savage309/HPC2015/blob/master/internal/homework.js
В случай, че някое домашно не се компилира, не се линква, crash-ва рънтайм или не работи правилно, вероятноста да получите 0 (НУЛА) точки е голяма.

