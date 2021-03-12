(function(){
    const IMAGE_WIDTH_HEADER = 'X-Image-Width'
    const IMAGE_HEIGHT_HEADER = 'X-Image-Height'

    const xsmallImgBtn = document.getElementById('my-btn-xsmall')
    const smallImgBtn = document.getElementById('my-btn-small')
    const mediumImgBtn = document.getElementById('my-btn-medium')
    const progressBar = document.getElementById('progress-bar')
    const progressBarText = document.getElementById('progress-bar-text')

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
        const size = width * height * 4

        const reader = res.body.getReader()
        let loaded = 0
        const arr = new Uint8ClampedArray(size)
        while (true) {
            const { done, value } = await reader.read()
            if (done) break
            for (let i = 0; i < value.length; i++) {
                arr[i + loaded] = value[i]
            }
            loaded += value.length
            const percentage = Math.round(100 * loaded / size)
            progressBar.style.width = `${percentage}%`
            progressBarText.textContent = `${percentage}% downloaded...`

            ctx.putImageData(new ImageData(arr, width, height), 0, 0)
        }
        progressBarText.textContent = 'done'
    }

    mediumImgBtn.addEventListener('click', () => fetchAndHandleImageStream('/raw_medium'))
    smallImgBtn.addEventListener('click', () => fetchAndHandleImageStream('/raw_small'))
    xsmallImgBtn.addEventListener('click', () => fetchAndHandleImageStream('/raw_xsmall'))
})()
