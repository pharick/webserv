# webserv

**Учебный проект HTTP-сервера на C++. Проект выполнен в Школе 21.**

## Основные особенности

- Обрабатывает соеденинения с клиентами используя мультиплексирование ввода/вывода (функцию poll)
- Раздает статические файлы
- Обрабатывает методы GET, HEAD, POST, PUSH
- Умеет обрабатывать chunked body
- Настраивается через конфигурационный файл
- Позволяет создавать несколько виртуальных серверов и настраивать обработку разных url в зависимости от префикса или расширения (location)
- Умеет передавать запросы на обработку cgi-скриптам

## Сборка и запуск

make && ./webserv [config_file]
