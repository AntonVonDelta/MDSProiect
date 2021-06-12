/**
 * Class used for loading and interacting with the rendered objects.
 * @author Vlad Pirlog <https://github.com/vladpirlog>
 * @see https://github.com/AntonVonDelta/MDSProiect
 */
class SceneRenderer {
    /**
     * Map for translating actions into API endpoints.
     * @private
     * @static
     */
    private static _ENDPOINTS = {
        LOGIN: '/api/login',
        MOVE: '/api/move',
        ROTATE: '/api/rotate',
        LOAD: '/api/load'
    }

    /**
     * RegExp object used for validating `.obj` input strings.
     * @private
     * @static
     */
    private static _OBJ_LINE_REGEX = /^(v( -?(0|[1-9]\d*)(\.\d+)?){3,4}|vn( -?(0|[1-9]\d*)(\.\d+)?){3}|vt( (1(\.0+)?|0(\.\d+)?)){1,3}|f( [1-9]\d*((\/([1-9]\d*)?)?\/[1-9]\d*)?){3,})$/

    /**
     * Map for translating moving directions into API opcodes.
     * @private
     * @static
     */
    private static _MOVE_DIRECTION_OPCODES = {
        positiveX: 2 as const,
        negativeX: 3 as const,
        positiveY: 4 as const,
        negativeY: 5 as const,
        positiveZ: 0 as const,
        negativeZ: 1 as const,
    }

    /**
     * Map for translating rotation directions into API opcodes.
     * @private
     * @static
     */
    private static _ROTATE_DIRECTION_OPCODES = {
        aroundX: 1 as const,
        aroundY: 0 as const
    }

    /**
     * Map used for acquiring the image size from the custom HTTP headers.
     * @private
     * @static
     */
    private static _IMAGE_SIZE_HEADERS = {
        WIDTH: 'X-Image-Width',
        HEIGHT: 'X-Image-Height'
    }

    /**
     * Flag used for signaling current user's authentication status.
     * @private
     */
    private _loggedIn = false

    /**
     * Function that authenticates the user in order to start the video feed.
     * @returns a promise that resolves to an object with `width`, `height` (the size of the image) and
     * `stream` (null or a ReadableStream for an UInt8Array) properties
     * @throws `AlreadyLoggedInError`, `CannotGetCookieError`, `UnknownStatusCodeError`
     * @async
     */
    public async login () {
        if (this._loggedIn) throw new AlreadyLoggedInError()

        const res = await fetch(SceneRenderer._ENDPOINTS.LOGIN, { credentials: 'same-origin' })
        if (res.status === 500) throw new InternalServerError(await res.text())
        if (res.status === 409) throw new CannotGetCookieError()
        if (res.status !== 200) throw new UnknownStatusCodeError()

        this._loggedIn = true
        const imageWidthHeader = res.headers.get(SceneRenderer._IMAGE_SIZE_HEADERS.WIDTH)
        const imageHeightHeader = res.headers.get(SceneRenderer._IMAGE_SIZE_HEADERS.HEIGHT)

        const imageWidth = imageWidthHeader ? parseInt(imageWidthHeader) : 720
        const imageHeight = imageHeightHeader ? parseInt(imageHeightHeader) : 480
        const imageReadableStream = res.body

        return { width: imageWidth, height: imageHeight, stream: imageReadableStream }
    }

    /**
     * Function that sends a `.obj` formatted string to the server in order to render it.
     * @param data a string representing the user input in a `.obj` format
     * @returns a promise that resolves to `true` if the request succeeded, else throws an error
     * @throws `UnauthorizedError`, `MalformedDataError`, `UnknownStatusCodeError`
     * @see https://www.cs.cmu.edu/~mbz/personal/graphics/obj.html
     * @async
     */
    public async loadFromText (data: string) {
        if (!this._loggedIn) throw new UnauthorizedError()

        const validLines = data
            .split('\n')
            .map(line => line.trim())
            .filter(line => line.length > 0 && !line.startsWith('#'))
        if (!validLines.every(line => SceneRenderer._OBJ_LINE_REGEX.test(line))) {
            throw new MalformedDataError()
        }

        const res = await fetch(SceneRenderer._ENDPOINTS.LOAD, {
            credentials: 'same-origin',
            body: data,
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' }
        })
        if (res.status === 500) throw new InternalServerError(await res.text())
        if (res.status === 404) {
            this._loggedIn = false
            throw new UnauthorizedError()
        }
        if (res.status === 422) {
            throw new MalformedDataError(await res.text())
        }
        if (res.status !== 200) throw new UnknownStatusCodeError()
        return true
    }

    /**
     * Function that sends a `move` command to the server in order to move the rendered object.
     * @param direction positive or negative movement along one of the 3 axes
     * @param amount number of units to move in the given direction
     * @returns a promise that resolves to `true` if the request succeeded, else throws an error
     * @throws `UnauthorizedError`, `UnknownStatusCodeError`
     * @async
     */
    public async move (direction: keyof typeof SceneRenderer._MOVE_DIRECTION_OPCODES, amount: number) {
        if (!this._loggedIn) throw new UnauthorizedError()

        const queryParams = new URLSearchParams()
        queryParams.append('direction', SceneRenderer._MOVE_DIRECTION_OPCODES[direction].toString())
        queryParams.append('amount', amount.toString())

        const fullPath = SceneRenderer._ENDPOINTS.MOVE + '&' + queryParams.toString()

        const res = await fetch(fullPath, { credentials: 'same-origin' })
        if (res.status === 500) throw new InternalServerError(await res.text())
        if (res.status === 404) {
            this._loggedIn = false
            throw new UnauthorizedError()
        }
        if (res.status !== 200) throw new UnknownStatusCodeError()
        return true
    }

    /**
     * Function that sends a `rotate` command to the server in order to rotate the rendered object.
     * @param direction the axis of rotation
     * @param amount number of degrees to rotate the object by; positive means counter-clockwise rotation
     * and negative means clockwise rotation
     * @returns a promise that resolves to `true` if the request succeeded, else throws an error
     * @throws `UnauthorizedError`, `UnknownStatusCodeError`
     * @async
     */
    public async rotate (direction: keyof typeof SceneRenderer._ROTATE_DIRECTION_OPCODES, amount: number) {
        if (!this._loggedIn) throw new UnauthorizedError()

        const queryParams = new URLSearchParams()
        queryParams.append('direction', SceneRenderer._ROTATE_DIRECTION_OPCODES[direction].toString())
        queryParams.append('amount', amount.toString())

        const fullPath = SceneRenderer._ENDPOINTS.ROTATE + '&' + queryParams.toString()

        const res = await fetch(fullPath, { credentials: 'same-origin' })
        if (res.status === 500) throw new InternalServerError(await res.text())
        if (res.status === 404) {
            this._loggedIn = false
            throw new UnauthorizedError()
        }
        if (res.status !== 200) throw new UnknownStatusCodeError()
        return true
    }
}

class UnauthorizedError extends Error {
    constructor (message = 'Unauthorized') {
        super(message)
    }
}

class AlreadyLoggedInError extends Error {
    constructor (message = 'Already Logged In') {
        super(message)
    }
}

class UnknownStatusCodeError extends Error {
    constructor (message = 'Unknown Status Code') {
        super(message)
    }
}

class CannotGetCookieError extends Error {
    constructor (message = 'Cannot Get Cookie') {
        super(message)
    }
}

class MalformedDataError extends Error {
    constructor (message = 'Malformed Data') {
        super(message)
    }
}

class InternalServerError extends Error {
    constructor (message = 'Internal Server Error') {
        super(message)
    }
}
