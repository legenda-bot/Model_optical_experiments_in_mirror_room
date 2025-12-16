# Обзор функций проекта «modelation_of_mirrors»

Краткие описания назначений основных функций по файлам.

## lightray.cpp / lightray.h
- `signedAngle(a, b)`: ориентированный угол между векторами.
- `onArc(center, a, b, p, useMinor)`: проверка, лежит ли точка p на выбранной дуге (короткой/длинной) окружности.
- `LightRay::LightRay(startPoint, startAngle, walls)`: конструктор, сохраняет параметры и строит путь.
- `calculatePath(maxReflections)`: трассировка луча с отражениями, формирует список точек пути.
- `findNextWall(currentPoint, currentAngle, intersection, skipWall)`: поиск ближайшей стены/дуги для пересечения в текущем направлении.
- `calculateReflection(currentPoint, currentAngle, wall, newAngle)`: вычисление отражённого направления от плоской или сферической стены.
- `path() const`: доступ к рассчитанному пути луча.

## wall.cpp / wall.h
- `Wall::Wall(start, end)`: создаёт стену-отрезок с типом Flat по умолчанию.
- `setMirrorType / setSphericalType / setRadius`: конфигурирование типа зеркала и радиуса.
- `mirrorType / sphericalType / radius / line / startPoint / endPoint / length`: геттеры свойств.
- `containsPoint(point)`: проверка, близка ли точка к стене (для выбора кликом).
- `draw(painter) const`: отрисовка стены, нормали/дуги для сферических зеркал.
- `reflectPoint(point) const`: упрощённое отражение точки относительно стены.
- `distanceToPoint(point) const`: расстояние от точки до отрезка стены.
- `getClosestPoint(point) const`: ближайшая точка на стене к заданной.
- `toString() const`: строковое описание стены.
- `setRoomCenter / hasRoomCenter / roomCenter`: установка/получение центра комнаты (для ориентации нормалей).

## mirrorroom.cpp / mirrorroom.h
- `MirrorRoom::MirrorRoom(parent) / ~MirrorRoom()`: создание/уничтожение виджета комнаты, настройка анимации.
- `setRoomCreationMode(mode)`: выбор способа создания комнаты (клик или правильный многоугольник).
- `setNumberOfWalls(count)`: установка числа стен для правильного многоугольника.
- `startRayExperiment(startPoint, angle)` / `startRayExperiment(angle)`: запуск трассировки луча от указанной или выбранной точки.
- `clearRay()`: сброс текущего луча и анимации.
- `clearRoom()`: очистка комнаты, стен, камеры и состояний выбора.
- `setPolygonScale(percent)`: масштаб правильного многоугольника при авто-построении.
- `setAnimationSpeed(milliseconds)`: скорость анимации луча.
- `saveExperiment / loadExperiment(filename)`: заглушки для сохранения/загрузки экспериментов.
- `getWall / getWalls`: доступ к стенам.
- `setSelectingStartPoint(selecting) / setSelectingAngle(selecting) / setCurrentAngle(angle)`: управление режимами выбора стартовой точки и угла.
- `getRayStartPoint / getCurrentAngle`: получение текущих настроек луча.
- Сигналы: `wallSelected`, `angleUpdated`, `simulationStateChanged`.
- `paintEvent`: отрисовка сцены (стены, луч, выбор угла/точки).
- `mousePressEvent / mouseMoveEvent / mouseReleaseEvent`: обработка кликов для построения комнаты, выбора точки/угла и панорамирования.
- `keyPressEvent`: зум по клавишам `+/-`.
- `createRegularPolygon()`: построение правильного n-угольника внутри виджета.
- `completeRoom()`: завершение построения комнаты, проверка выпуклости, создание стен, установка центра.
- `drawWalls(painter) / drawRay(painter) / drawAngleSelection(painter) / drawStartPointAndTrajectory(painter)`: вспомогательные функции отрисовки.
- `findWallAtPoint(point) const`: поиск стены под курсором.
- `calculateAngle(start, end) const`: угол в градусах по двум точкам.
- `calculateDirectionVector(angle) const`: единичный вектор по углу (градусы).
- `advanceRayAnimation()`: пошаговая анимация построенного пути луча.
- `rebuildSegmentDurations()`: пересчёт длительности анимации по длине сегментов.
- `updateViewTransform()`: сбор матрицы вида из центра камеры и масштабирования.
- `adjustZoom(factor, focusPoint)`: изменение масштаба с сохранением точки фокуса под курсором.
- `ensureCameraInitialized()`: ленивое задание центра камеры.
- `isConvexPolygon(points) const`: проверка выпуклости набора точек.
- `calculateRegularPolygonPoint(index, totalPoints, area)`: координаты вершины правильного многоугольника в заданной области.
