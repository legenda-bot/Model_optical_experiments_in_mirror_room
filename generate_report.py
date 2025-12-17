from __future__ import annotations

from datetime import date
from pathlib import Path

from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import cm
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.platypus import PageBreak, Paragraph, SimpleDocTemplate, Spacer


def register_fonts() -> str:
    font_candidates = [
        r"C:\Windows\Fonts\arial.ttf",
        r"C:\Windows\Fonts\DejaVuSans.ttf",
        r"C:\Windows\Fonts\calibri.ttf",
    ]
    for font_path in font_candidates:
        if Path(font_path).exists():
            pdfmetrics.registerFont(TTFont("BodyFont", font_path))
            return "BodyFont"

    raise FileNotFoundError("Не найден подходящий шрифт с поддержкой кириллицы (например Arial).")


def build_story(font_name: str) -> list:
    styles = getSampleStyleSheet()
    base = ParagraphStyle(
        name="Base",
        parent=styles["Normal"],
        fontName=font_name,
        fontSize=12,
        leading=15,
        spaceAfter=8,
    )
    title = ParagraphStyle(
        name="Title",
        parent=base,
        fontSize=18,
        leading=22,
        alignment=1,  # center
        spaceAfter=18,
    )
    h1 = ParagraphStyle(
        name="H1",
        parent=base,
        fontSize=14,
        leading=18,
        spaceBefore=12,
        spaceAfter=10,
    )
    mono = ParagraphStyle(
        name="Mono",
        parent=base,
        fontName=font_name,
    )

    def p(text: str, style=base):
        return Paragraph(text, style)

    story: list = []

    story += [
        Spacer(1, 5 * cm),
        p("Отчёт по проекту", title),
        p("Модель оптических экспериментов в зеркальной комнате", title),
        Spacer(1, 2 * cm),
        p("<b>Авторы:</b> Касаткин Александр, Фатыхов Арон, 11 И", base),
        p(f"<b>Дата:</b> {date.today().strftime('%d.%m.%Y')}", base),
        PageBreak(),
    ]

    story += [
        p("1. Введение", h1),
        p(
            "Проект представляет собой интерактивное приложение, моделирующее распространение светового луча "
            "в замкнутой «зеркальной комнате». Пользователь может создать многоугольную комнату, задать "
            "тип стен (плоские или сферические), выбрать стартовую точку луча и направление, после чего "
            "программа вычислит последовательность отражений и отрисует траекторию.",
            base,
        ),
        p("2. Цель и задачи", h1),
        p(
            "<b>Цель:</b> реализовать наглядную симуляцию отражения луча от зеркальных поверхностей в замкнутой области.",
            base,
        ),
        p(
            "<b>Задачи:</b> (1) создать интерфейс для построения комнаты и задания параметров эксперимента; "
            "(2) реализовать вычисление пересечений и отражений; "
            "(3) обеспечить корректную визуализацию и анимацию траектории; "
            "(4) добавить сохранение/загрузку экспериментов.",
            base,
        ),
        p("3. Технологии", h1),
        p(
            "Приложение написано на C++ (C++17) с использованием Qt (Widgets) для интерфейса и отрисовки. "
            "Геометрические вычисления вынесены в отдельный модуль (логический backend), "
            "не зависящий от Qt.",
            base,
        ),
        PageBreak(),
    ]

    story += [
        p("4. Структура проекта", h1),
        p(
            "Проект условно разделён на две части: UI (Qt) и геометрический backend. "
            "UI отвечает за ввод параметров, взаимодействие с пользователем и отрисовку. "
            "Backend отвечает за «чистую» геометрию: пересечения, отражения и построение траектории.",
            base,
        ),
        p("<b>UI-часть (Qt):</b>", h1),
        p(
            "<b><font face='BodyFont'>main.cpp</font></b> — запуск приложения и создание главного окна.",
            mono,
        ),
        p(
            "<b><font face='BodyFont'>mainwindow.*</font></b> — панель управления: режим создания комнаты, "
            "угол, скорость, кнопки запуска/очистки, сохранение/загрузка.",
            mono,
        ),
        p(
            "<b><font face='BodyFont'>mirrorroom.*</font></b> — основной виджет сцены: хранит стены, "
            "обрабатывает события мыши/клавиш, рисует комнату и траекторию, управляет анимацией.",
            mono,
        ),
        p(
            "<b><font face='BodyFont'>wall.*</font></b> — модель стены в UI (тип Flat/Spherical, "
            "Concave/Convex, радиус) и отрисовка/хит‑тест.",
            mono,
        ),
        p(
            "<b><font face='BodyFont'>lightray.*</font></b> — построение траектории и передача списка точек "
            "в UI для отрисовки.",
            mono,
        ),
        p("<b>Backend (геометрия, без Qt):</b>", h1),
        p(
            "<b><font face='BodyFont'>backend/BackendWall.*</font></b> — «стена» как сегмент или дуга, "
            "поиск пересечений и отражение луча.",
            mono,
        ),
        p(
            "<b><font face='BodyFont'>backend/BackendPolygon.*</font></b> (BackendMirrorRoom) — набор стен и "
            "алгоритм трассировки луча с многократными отражениями.",
            mono,
        ),
        p(
            "<b><font face='BodyFont'>backend/ExperimentDatabase.*</font></b> — in-memory хранилище экспериментов.",
            mono,
        ),
        p(
            "Низкоуровневая геометрия реализована в файлах: Point/Vector/Line/Segment/Ray/Circle/Arc/Intersections. "
            "Они задают примитивы и операции пересечения/отражения, которыми пользуется backend.",
            base,
        ),
        PageBreak(),
    ]

    story += [
        p("5. Что происходит при запуске моделирования", h1),
        p(
            "Моделирование запускается из интерфейса кнопкой «Start Ray Tracing». "
            "Далее происходит следующий процесс:",
            base,
        ),
        p(
            "<b>Шаг 1 — сбор параметров.</b> MainWindow передаёт в MirrorRoom стартовую точку и угол.",
            base,
        ),
        p(
            "<b>Шаг 2 — расчёт траектории.</b> MirrorRoom создаёт объект LightRay. "
            "В конструкторе LightRay вызывается расчёт траектории.",
            base,
        ),
        p(
            "<b>Шаг 3 — конвертация UI → backend.</b> Каждая стена Wall преобразуется в BackendWall: "
            "плоская стена становится сегментом (Segment), сферическая — дугой (Arc) по трём точкам "
            "(start/middle/end). Для дуги middle вычисляется из радиуса и ориентации concave/convex "
            "относительно центра комнаты.",
            base,
        ),
        p(
            "<b>Шаг 4 — трассировка (backend).</b> BackendMirrorRoom::FireBeam на каждом шаге ищет ближайшее "
            "пересечение текущего луча со всеми стенами, добавляет точку удара в путь и вычисляет отражённый луч. "
            "Если пересечения отсутствуют — считается, что луч покинул комнату.",
            base,
        ),
        p(
            "<b>Шаг 5 — визуализация.</b> Полученный список точек переводится обратно в QPointF и хранится как путь. "
            "Дальше UI не пересчитывает геометрию, а только анимирует рисование сегментов пути во времени.",
            base,
        ),
        PageBreak(),
    ]

    story += [
        p("6. Пользовательский сценарий", h1),
        p(
            "1) Создать комнату (ручным кликом или регулярным многоугольником). "
            "2) Выбрать стену и задать тип зеркала (Flat/Spherical), для сферического — радиус и тип (Concave/Convex). "
            "3) Выбрать стартовую точку луча на стене/дуге и задать угол. "
            "4) Нажать «Start Ray Tracing» и наблюдать траекторию. "
            "5) При необходимости сохранить эксперимент в файл и загрузить его позже.",
            base,
        ),
        p("7. Ограничения и возможные улучшения", h1),
        p(
            "— Траектория вычисляется целиком при старте, поэтому при очень большом лимите отражений возможны задержки. "
            "— Для повышения точности можно дополнительно улучшать устойчивость к погрешностям (eps‑порогами) "
            "и обработку «граничных» попаданий в вершины. "
            "— Можно добавить экспорт траектории и параметров в отчёт/изображение.",
            base,
        ),
    ]

    return story


def main() -> None:
    out_path = Path(__file__).with_name("Отчет_Модель_оптических_экспериментов_в_зеркальной_комнате.pdf")
    font_name = register_fonts()

    doc = SimpleDocTemplate(
        str(out_path),
        pagesize=A4,
        leftMargin=2 * cm,
        rightMargin=2 * cm,
        topMargin=2 * cm,
        bottomMargin=2 * cm,
        title="Отчёт по проекту",
        author="Касаткин Александр; Фатыхов Арон",
    )

    doc.build(build_story(font_name))
    print(f"OK: {out_path}")


if __name__ == "__main__":
    main()

