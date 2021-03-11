const express = require('express')
const app = express()

app.use(express.static(__dirname + '/static'))

app.get('/raw_medium', (req, res, next) => {
    res.setHeader('X-Image-Width', '1920')
    res.setHeader('X-Image-Height', '1280')

    res.sendFile(__dirname + '/sample_medium.rgba')
})

app.get('/raw_small', (req, res, next) => {
    res.setHeader('X-Image-Width', '1280')
    res.setHeader('X-Image-Height', '853')

    res.sendFile(__dirname + '/sample_small.rgba')
})

app.get('/raw_xsmall', (req, res, next) => {
    res.setHeader('X-Image-Width', '300')
    res.setHeader('X-Image-Height', '300')

    res.sendFile(__dirname + '/sample_xsmall.rgba')
})

app.use((req, res, next) => res.sendStatus(404))

app.listen(8000, () => console.log('Listening on port 8000...'))
