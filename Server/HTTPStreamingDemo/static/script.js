(function(){
    const IMAGE_WIDTH_HEADER = 'X-Image-Width'
    const IMAGE_HEIGHT_HEADER = 'X-Image-Height'

    const xsmallImgBtn = document.getElementById('my-btn-xsmall')
    const smallImgBtn = document.getElementById('my-btn-small')
    const mediumImgBtn = document.getElementById('my-btn-medium')

    /**
     * @type {HTMLCanvasElement}
     */
    const cnv = document.getElementById('my-cnv')
    const ctx = cnv.getContext('2d')

    const fetchAndHandleImageStream = async path => {
        ctx.clearRect(0, 0, cnv.width, cnv.height)
        const res = await fetch(path)

        const width = parseInt(res.headers.get(IMAGE_WIDTH_HEADER)) || 720
        const height = parseInt(res.headers.get(IMAGE_HEIGHT_HEADER)) || 480

        const reader = res.body.getReader()
        let loaded = 0
        const arr = new Uint8ClampedArray(width * height * 4)
        while (true) {
            const { done, value } = await reader.read()
            if (done) break
            for (let i = 0; i < value.length; i++) {
                arr[i + loaded] = value[i]
            }
            loaded += value.length
            console.log(`${loaded} bytes downloaded...`)
            ctx.putImageData(new ImageData(arr, width, height), 0, 0)
        }
    }

    mediumImgBtn.addEventListener('click', () => fetchAndHandleImageStream('/raw_medium'))
    smallImgBtn.addEventListener('click', () => fetchAndHandleImageStream('/raw_small'))
    xsmallImgBtn.addEventListener('click', () => fetchAndHandleImageStream('/raw_xsmall'))

})()
